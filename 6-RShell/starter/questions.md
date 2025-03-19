1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client detects the end of output using an eof marker (like 0x04) or length-prefixed messages. Since TCP transmits data as a continuous stream, the client must handle partial reads by repeatedly calling recv() until the full message is received. Techniques include looped recv() that read until eof, length-prefixed protocols that read the message size and then then fetch number of bytes, and  fixed-length messages that process data in predefined chunks.


2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

The boundaries of a shell protocol must be explicitly marked through null-terminator and length-prefixed messages. If no clear boundaries are made, the message might be fragmented wrongly, misinterpreted, or parsed poorly. 

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols maintain a session or connection state between a client and the server. every request depends on previous interactions. some examples would be TCP, SSH, FTP. On one hand there is security due to authentication, and real-time communication is possible. however it might be costly 

Stateless protocols do not retain information between their requests, so every new one is independent. For example we have UDP, HTTPS, DNS. These are much more resourceful in terms of memory but the con is that the context has to be provided during every interaction. 

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Even though UDP does not guarantee order or error correction, it is useful because of its low latency which makes it faster for applications and sending data. It also uses considerably less system resources as was mentioned above

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The abstraction is sockets. Through them we can create endpoints, bind them with an address, manage and communicate with both TCP and UDP connections, and etc. 