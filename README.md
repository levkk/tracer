# Tracer

This little library can be injected into any application to trace
network calls.

### Example

```bash
make python
rm -f tracer.so
g++ -Wall -fPIC -shared -o tracer.so tracer.cpp -ldl
LD_PRELOAD=./tracer.so python3
Python 3.8.10 (default, Sep 28 2021, 16:10:42)
[GCC 9.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import requests
>>> requests.get("https://google.com")

=== Tracer stats ===
IP 172.217.5.110 received a total of 569 bytes.
IP 172.217.5.110 sent a total of 7931 bytes.

=== Tracer stats ===
IP 172.217.5.100 received a total of 577 bytes.
IP 172.217.5.100 sent a total of 12547 bytes.
<Response [200]>
>>>
```