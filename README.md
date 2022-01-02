# Tracer

This little library can be injected into any application to trace
network calls.

### Example

Make sure to have `requests` installed in your Python packages.

```bash
make python
rm -f tracer.so
g++ -Wall -fPIC -shared -o tracer.so tracer.cpp -ldl
LD_PRELOAD=./tracer.so python3 -c "import requests; requests.get('https://google.com')"

=== Tracer stats ===
IP 142.250.188.14 received a total of 569 bytes.
IP 142.250.188.14 sent a total of 7932 bytes.

=== Tracer stats ===
IP 142.250.189.164 received a total of 577 bytes.
IP 142.250.189.164 sent a total of 12734 bytes.
```


### Limitations
Only works on UNIX. Windows support will require using `GetProcAddress` ([docs](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress?redirectedfrom=MSDN)).