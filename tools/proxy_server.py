#!/usr/bin/env python3
"""
HTTP代理服务器
功能：接收HTTP请求，转发到HTTPS API，返回响应
"""

import http.server
import socketserver
import json
import urllib.request
import urllib.parse
import ssl
import sys
from http import HTTPStatus

class ProxyHandler(http.server.BaseHTTPRequestHandler):
    """处理代理请求"""
    
    def do_POST(self):
        """处理POST请求"""
        if self.path != '/proxy':
            self.send_error(404, "Not Found")
            return
        
        # 读取请求体
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length)
        
        try:
            # 解析代理请求
            proxy_request = json.loads(body.decode('utf-8'))
            
            # 提取请求参数
            method = proxy_request.get('method', 'POST')
            url = proxy_request.get('url', '')
            headers = proxy_request.get('headers', {})
            request_body = proxy_request.get('body', '')
            
            if not url:
                raise ValueError("Missing URL")
            
            print(f"代理请求: {method} {url}")
            
            # 处理请求体（可能是字典或字符串）
            request_body_data = None
            if request_body:
                if isinstance(request_body, dict):
                    # 如果是字典，转换为JSON字符串
                    request_body_data = json.dumps(request_body).encode('utf-8')
                elif isinstance(request_body, str):
                    # 如果是字符串，直接编码
                    request_body_data = request_body.encode('utf-8')
                else:
                    # 其他类型，尝试转换为字符串
                    request_body_data = str(request_body).encode('utf-8')
            
            # 创建HTTPS请求
            req = urllib.request.Request(
                url=url,
                data=request_body_data,
                headers=headers,
                method=method
            )
            
            # 发送请求（忽略SSL证书验证，仅用于测试）
            context = ssl._create_unverified_context()
            
            try:
                with urllib.request.urlopen(req, context=context, timeout=30) as response:
                    # 读取响应
                    response_data = response.read()
                    response_headers = dict(response.headers)
                    
                    # 发送响应
                    self.send_response(response.status)
                    
                    # 复制必要的头部
                    for key, value in response_headers.items():
                        if key.lower() not in ['transfer-encoding', 'connection']:
                            self.send_header(key, value)
                    
                    self.send_header('Content-Length', str(len(response_data)))
                    self.send_header('Connection', 'close')
                    self.end_headers()
                    
                    # 发送响应体
                    self.wfile.write(response_data)
                    
                    print(f"代理成功: HTTP {response.status}")
                    
            except urllib.error.HTTPError as e:
                # HTTP错误
                error_data = e.read()
                self.send_response(e.code)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Content-Length', str(len(error_data)))
                self.send_header('Connection', 'close')
                self.end_headers()
                self.wfile.write(error_data)
                print(f"代理HTTP错误: {e.code}")
                
            except Exception as e:
                # 其他错误
                error_msg = json.dumps({"error": str(e)}).encode('utf-8')
                self.send_response(500)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Content-Length', str(len(error_msg)))
                self.send_header('Connection', 'close')
                self.end_headers()
                self.wfile.write(error_msg)
                print(f"代理错误: {e}")
                
        except json.JSONDecodeError:
            error_msg = json.dumps({"error": "Invalid JSON"}).encode('utf-8')
            self.send_response(400)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Content-Length', str(len(error_msg)))
            self.send_header('Connection', 'close')
            self.end_headers()
            self.wfile.write(error_msg)
            print("代理错误: 无效的JSON")
            
        except Exception as e:
            error_msg = json.dumps({"error": str(e)}).encode('utf-8')
            self.send_response(500)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Content-Length', str(len(error_msg)))
            self.send_header('Connection', 'close')
            self.end_headers()
            self.wfile.write(error_msg)
            print(f"代理错误: {e}")
    
    def do_GET(self):
        """处理GET请求（健康检查）"""
        if self.path == '/health':
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Connection', 'close')
            self.end_headers()
            response = json.dumps({"status": "ok", "service": "ai-proxy"}).encode('utf-8')
            self.wfile.write(response)
        else:
            self.send_error(404, "Not Found")
    
    def log_message(self, format, *args):
        """自定义日志格式"""
        print(f"[{self.address_string()}] {format % args}")

def run_proxy(port=8080):
    """启动代理服务器"""
    handler = ProxyHandler
    with socketserver.TCPServer(("", port), handler) as httpd:
        print(f"代理服务器启动在 http://localhost:{port}")
        print(f"代理端点: POST http://localhost:{port}/proxy")
        print(f"健康检查: GET http://localhost:{port}/health")
        print("按 Ctrl+C 停止服务器")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n停止代理服务器")
            httpd.shutdown()

if __name__ == "__main__":
    port = 8080
    if len(sys.argv) > 1:
        try:
            port = int(sys.argv[1])
        except ValueError:
            print(f"Usage: {sys.argv[0]} [port]")
            sys.exit(1)
    
    run_proxy(port)