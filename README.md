# Server-Client-Socket-Programming
CN Project7, ZJU



Server file

/tcpcliserv/mytcpserv.c

Use with client:

/tcpcliserv/tcpcli01.c



## Usage

Start Server

```
tcpserv01 &
```

Start Client

```
tcpcli01 127.0.0.1
```

(May require sudo)



## Grammar

(Input at client)

```
t
-response current time

n
-response server's name

l (TODO)
-response client list

s <client serial> <message>
-send <message> to client with  <client serial>
-for invalid serial, response "Invalid"
```





