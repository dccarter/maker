
local Http = import('sys/http')
print(Lush.OS_VARIANT)
local resp = Http("https://httpbin.org/get", {})
print(resp.body)
