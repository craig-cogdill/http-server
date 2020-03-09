# http-server

Assumptions:
- Each string in the first line of the request is separated by a single space
- Any request that doesn't specify HTTP version 1.1 is invalid
- Request will be generated by a valid HTTP client
    - Malformed header strings such as "Content-Length= 8" or "Content-Length:8" will not be used