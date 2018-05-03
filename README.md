Connected Quiz
==============

Simple quiz game with server and client parts made in order to make the game more approachable by many players.

Description
-----------

Server part of the game is using `fork()` in order to allow multi client possibilities. Parent process which is responsible for calculating the score has 20 child processes (maximum number of possible connections at one time), which will handle the connected clients, passing the points to the parent using `pipe()`.  All of the messages from the server have to be verified by client by sending back the control message (`OK`). Then the traffic between client and server can look just like this:

`Where would you find the Sea of Tranquility?OKMarsOKTitanOKVenusOKMoonOK4 Correct answer!OK`

To-Do List
----------

* Menu for client allowing to change IP address of the client.
* Score is also passed to clients.
* Code review.
* Some sort of the verification, is the client proper version?
* More scalable solution to multi client server, probably events?

Usage
-----

* Run server `./server`
* Run client `./client`

License
-------

Permission to use, copy, modify, and distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
