package.cpath = package.cpath .. ";core.dll"
local socket = require("socket.core")
host = host or "127.0.0.1"
port = port or 8888
-- if arg then
    -- host = arg[1] or host
    -- port = arg[2] or port
-- end
print("Binding to host '" ..host.. "' and port " ..port.. "...")
udp = assert(socket.udp())
assert(udp:setsockname(host, port))
assert(udp:settimeout(5))
ip, port = udp:getsockname()
assert(ip, port)
print("Waiting packets on " .. ip .. ":" .. port .. "...")
while 1 do
	dgram, ip, port = udp:receivefrom()
	if dgram then
		print("Echoing '" .. dgram .. "' to " .. ip .. ":" .. port)
		udp:sendto(dgram, ip, port)
	else
        print(ip)
    end
end
