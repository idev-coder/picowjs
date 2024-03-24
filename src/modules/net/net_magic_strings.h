/* Copyright (c) 2024 Pico-W-JS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __NET_MAGIC_STRINGS_H
#define __NET_MAGIC_STRINGS_H

#define MSTR_NET "net"

#define MSTR_NET_BLOCKLIST "BlockList"
#define MSTR_NET_BLOCKLIST_ADD_ADDRESS "addAddress"
#define MSTR_NET_BLOCKLIST_ADD_RANGE "addRange"
#define MSTR_NET_BLOCKLIST_ADD_SUBNET "addSubnet"
#define MSTR_NET_BLOCKLIST_CHECK "check"
#define MSTR_NET_BLOCKLIST_RULES "rules"

#define MSTR_NET_SOCKETADDRESS "SocketAddress"
#define MSTR_NET_SOCKETADDRESS_ADDRESS "address"
#define MSTR_NET_SOCKETADDRESS_FAMILY "family"
#define MSTR_NET_SOCKETADDRESS_FLOWLABEL "flowlabel"
#define MSTR_NET_SOCKETADDRESS_PORT "port"

#define MSTR_NET_SERVER "Server"
#define MSTR_NET_SERVER_ADDRESS "address"
#define MSTR_NET_SERVER_CLOSE "close"
#define MSTR_NET_SERVER_GET_CONNECTIONS "getConnections"
#define MSTR_NET_SERVER_LISTEN "listen"
#define MSTR_NET_SERVER_LISTENING "listening"
#define MSTR_NET_SERVER_MAX_CONNECTIONS "maxConnections"
#define MSTR_NET_SERVER_REF "ref"
#define MSTR_NET_SERVER_UNREF "unref"

#define MSTR_NET_SOCKET "Socket"
#define MSTR_NET_SOCKET_ADDRESS "address"
#define MSTR_NET_SOCKET_AUTO_SELECT_FAMILY_ATTEMPTED_ADDRESSES "autoSelectFamilyAttemptedAddresses"
#define MSTR_NET_SOCKET_BUFFER_SIZE "bufferSize"
#define MSTR_NET_SOCKET_BYTES_READ "bytesRead"
#define MSTR_NET_SOCKET_BYTES_WRITTEN "bytesWritten"
#define MSTR_NET_SOCKET_CONNECT "connect"
#define MSTR_NET_SOCKET_CONNECTING "connecting"
#define MSTR_NET_SOCKET_DESTROY "destroy"
#define MSTR_NET_SOCKET_DESTROYED "destroyed"
#define MSTR_NET_SOCKET_DESTROY_SOON "destroySoon"
#define MSTR_NET_SOCKET_END "end"
#define MSTR_NET_SOCKET_LOCAL_ADDRESS "localAddress"
#define MSTR_NET_SOCKET_LOCAL_PORT "localPort"
#define MSTR_NET_SOCKET_LOCAL_FAMILY "localFamily"
#define MSTR_NET_SOCKET_PAUSE "pause"
#define MSTR_NET_SOCKET_PENDING "pending"
#define MSTR_NET_SOCKET_REF "ref"
#define MSTR_NET_SOCKET_REMOTE_ADDRESS "remoteAddress"
#define MSTR_NET_SOCKET_REMOTE_FAMILY "remoteFamily"
#define MSTR_NET_SOCKET_REMOTE_PORT "remotePort"
#define MSTR_NET_SOCKET_RESET_AND_DESTROY "resetAndDestroy"
#define MSTR_NET_SOCKET_RESUME "resume"
#define MSTR_NET_SOCKET_SET_ENCODING "setEncoding"
#define MSTR_NET_SOCKET_SET_KEEP_ALIVE "setKeepAlive"
#define MSTR_NET_SOCKET_SET_NO_DELAY "setNoDelay"
#define MSTR_NET_SOCKET_SET_TIMEOUT "setTimeout"
#define MSTR_NET_SOCKET_TIMEOUT "timeout"
#define MSTR_NET_SOCKET_UNREF "unref"
#define MSTR_NET_SOCKET_WRITE "write"
#define MSTR_NET_SOCKET_READY_STATE "readyState"

#define MSTR_NET_CONNECT "connect"
#define MSTR_NET_CREATE_CONNECTION "createConnection"
#define MSTR_NET_CREATE_SERVER "createServer"
#define MSTR_NET_GET_DEFAULT_AUTO_SELECT_FAMILY "getDefaultAutoSelectFamily"
#define MSTR_NET_SET_DEFAULT_AUTO_SELECT_FAMILY "setDefaultAutoSelectFamily"
#define MSTR_NET_GET_DEFAULT_AUTO_SELECT_FAMILY_ATTEMPT_TIMEOUT "getDefaultAutoSelectFamilyAttemptTimeout"
#define MSTR_NET_SET_DEFAULT_AUTO_SELECT_FAMILY_ATTEMPT_TIMEOUT "setDefaultAutoSelectFamilyAttemptTimeout"
#define MSTR_NET_IS_IP "isIP"
#define MSTR_NET_IS_IP_v4 "isIPv4"
#define MSTR_NET_IS_IP_v6 "isIPv6"

#endif /* __NET_MAGIC_STRINGS_H */
