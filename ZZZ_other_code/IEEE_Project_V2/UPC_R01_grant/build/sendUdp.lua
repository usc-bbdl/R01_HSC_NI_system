package.cpath = package.cpath .. ";core.dll"
local socket = require("socket.core")
print(socket._VERSION)
--print(cside.sendNum) --13
-- local socket = require("socket")
host = host or "192.168.0.2" 
--"localhost"
port = port or 8888
-- if arg then
    -- host = arg[1] or host
    -- port = arg[2] or port
-- end
host = socket.dns.toip(host)
udp = assert(socket.udp())
assert(udp:setpeername(host, port))
print("Using remote host '" ..host.. "' and port " .. port .. "...")
-- while 1 do
print(cside.myVar)
-- line = "17"
-- if not line or line == "" then os.exit() end
assert(udp:send(line))
-- dgram = assert(udp:receive())
-- print(dgram)
-- end
