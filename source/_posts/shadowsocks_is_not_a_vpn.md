title: Shadowsocks：一个混淆工具
date: 2015-08-31 23:27:43
tags: shadowsocks
categories: 网络
---

shadowsocks是被设计来混淆数据，增加某Wall检查出流量特征所需的计算量，提高实时检测的成本，而不是加密。ss的作者多次强调过这一点。

说这些并不是指责ss的安全性，而是再三强调不要忘记ss作者的本意——这是一个帮你能用Google又或者是看别的什么奇怪porn网站的混淆工具，其意义是瞒过Wall的实时流量检测，而不是瞒过Wall后面的master minds。ss不是一个真正意义上的VPN，它无法保护你的数据安全。
<!-- more -->

不应该在 SS 里找“安全”。 SS 也是几个程序员的个人业余项目，没有经过严谨的密码学设计，甚至设计初衷就不包括安全性。然而，只要GFW不抽风自己实现加密算法并且写出大bug，AES 比 RC4 安全几个数量级应该是没有争议的。而明文 IV 不应该是问题，至多损失了 defense by depth ，不至于抹平这两者安全性的本质差异。

SS的用途不是加密，而是混淆。但是仍然可以暴力破解，你设置数字密码一样是不行的。若读懂shadowsocks，并改进过程序，使IV不一样就分析不出差别来，可以抵御基于GFW统计学的计算。

在通常应用中，SS主要是提供了一个不会被干扰的通道，你SS里面走的东西，一般都还有自己的TLS吧？ 如果VPN不会被干扰，谁会用这个东西。

## Title : Correct username/password auth model

[原文地址](https://github.com/shadowsocks/shadowsocks/issues/169)

### *==clowwindy== commented on 9 Aug 2014*

Original protocol:

	IV + addr + data
    
Someone implemented a [username auth version](https://github.com/Lupino/shadowsocks-auth), which is totally WRONG.

Version numbers and usernames are sent via plain text, which is too obvious for any firewall to detect.

	ver + namelen + username + IV + addr + data
    
Where ver, namelen, username are plain text.

We should implement a correct auth model:

	IV1 + ver + userhash + IV2 + addr + data
    
ver + userhash is encrypted using key, data is encrypted using user's password.

userhash is generated from username and password, with fixed length.

Also we should remove table encryption on this version.

Compatibility:

For one user: 

* compatible with current config.json

For multiple users:

* server config uses users: {user1: password1} format. port_password is no longer supported

* client config uses common_key, username, password

One problem:

If we implement this correct model, every user needs two passwords to use shadowsocks.

### *==613038475== commented on 27 Aug 2014*

The problem is about how to send user id.

Send it with fixed key can let it decryptable by other user (only the username, the rest data is safe).

But if send it with the first password (if there two) will cause performance problem because you don't known which user should choose to decrypt.

You may think about use asymmetric encryption.
[Diffie–Hellman key exchange](https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange)

a. server generate
generate private number
generate 2048bit prime number
generate 2048bit base number
server public key = base number ^ private number % prime number

b. distribute server public key, base number and prime to every client

c. client before connect
generate private number (random)
client public key = base number ^ private number % prime number
secret key = server public key ^ private number % prime number
encrypt username with secret key

d. client send public key and encrypted username to server

e. server after receive
secret key = client public key ^ private number % prime number
decrypt username with secret key
find password from this username

then use password to encrypt/decrypt rest data.

### *==clowwindy== commented on 27 Aug 2014*

@613038475
We don't need security. We need indistinguishability from random bytes. If we only need security we can just transfer username in plain text.

I want zero key exchange time so I choose PSK. Key exchanging sucks:

       |---------------------->|                      |
       | 4) Client Hello       |                      |
       |                       |                      |
       |<----------------------|                      |
       | 5) Server Hello       |                      |
       |                       |                      |
       |---------------------->|                      |
       | 6) Client Key Exchange|                      |
       |                       |                      |
       |---------------------->|                      |
       | 7) Client Finished    |                      |
       |                       |                      |
       |<----------------------|                      |
       | 8) Server Finished    |                      |
       |                       |                      |
       
### *==613038475== commented on 27 Aug 2014*

Hmm.. read my solution again.
Because client generate random power each times, the first 2048bit is almost random (not detectable).
In my solution server already distributed it's public key, so key exchange is half done.
You can notice steps is not increased.
a. client send public key, username, address to server
b. server decrypt username to find the password, then use password to decrypt address
c. transfer rest data use the key from the password
Also this one don't need two password and always find the password by O(1).

### *==clowwindy== commented on 27 Aug 2014*

Sorry, I misunderstood your meaning. This is practically safer than using two PSKs. But still, in the client config, you have to set:

username: xxx
password: xxx
pubkey: xxx

Users don't understand why there're two passwords. If we can find out a way to reduce password and pubkey to just one, while username is still encrypted somehow with randomness, but server knows how to decrypted it in O(1), it is much easier for our users.

This can be explained as:

* User Alice knows her username and her secret key.
* User Carol also knows his username and his secret key.
* There're also other users like Alice and Carol.
* Bob knows all usernames and secret keys, but he shares no other knowledge with all the users.
* Alice sends a message in cipher text to Bob.
* Carol sends a message in cipher text to Bob.
* Bob knows which username the cipher texts belong to in O(1) time. He can decrypt the cipher texts.
* The eavesdropper Eve does not know which cipher text belongs to which user.
* (Optional) Alice and Carol also can not tell which cipher text belongs to which user, if it is not their own cipher texts.

I'm not good at cryptography, therefore I can neither figure out a solution nor prove this is theoretical impossible yet. If this is impossible, we might compromise using two keys, no matter two PSKs or DH + PSK.

### *librehat commented on 12 Dec 2014*
I believe there is no need to have lots of keys. We only need one key for one server, which serves as master key to encrypt all data. So the config file would something like:

    server:"localhost",
    server_port:8388,
    ......
    pubkey:"pubkeyfile",
    users: {
    "alice":"alicepassword",
    "bob":"bobpassword"
    }
    
The pubkey can even be stored in the code if someone want to make a commercial product (that's another story).

### *caizixian commented on 29 Jan*

Username support is not a good idea because under most circumstances we just don't need username-password pair.
Port number-password pair is good enough.

### *clowwindy commented on 3 Mar*

Maybe we can learn from WPA Enterprise.

### *lepasserby commented on 26 Apr*

@clowwindy:
By the way, have you considered using something like ScrambleSuit or the like ?

### *clowwindy commented on 26 Apr*

@lepasserby 
Then the firewall could just decrypt ver and userhash using IV1 to find it's not random. It could block the connection then.

Speaking of ScrambleSuit, most people doing anti-censorship research outside China often don't realize that actually we don't care about censorship. They thought we're curious about uncensored news, and we're eager to find out what the government has blocked recently. Unfortunately that's not true. Nobody is gonna do that. Most people just want to watch silly gameplay videos on YouTube, share their silly photos on Instagram, download pirate or porn movies, play online PC or PS4 games that are often too slow without using a proxy. If we really were a freedom loving nation, we won't have such a firewall.

Compared to those projects from outsiders, the tools that Chinese people invented have one characteristic in common: speed. We have high goodput and low latency, even better than most of the VPN protocols.

### *lepasserby commented on 26 Apr*

@clowwindy 
good point re: decryption.

You're right that if we're aiming for indistinguishability against adversaries determined enough to run different decryption attempts on suspicious traffic (that's expensive because there probably are tens, if not hundreds, possible other "suspect sources" besides shadowsocks, but GFW in general is expensive, so there :) ) your two-password proposal is probably one of most simple and efficient solutions.

So,let's try to make it a little more efficient still.

We now have two pre-shared secrets : user_password and server_password (it would be prudent to have them different, so if user tries to have "user_password == server_password", we should at least give a fair warning and advice to make them at least slightly different)

Because user-generated passwords are usually rather crappy, we should probably derive keys from them. Since this is a one-time affair (reasonable implementations of both client and server should store the derived keys long-term after being supplied with relevant user_password and server_password) we should probably use a really modern secure hash here.
(The GFW people could still try creating a giant rainbow table of all likely user_password/username/server_password combinations, but at this point the user has to "merely" use good, long, high-entropy passwords to thwart such shenanigans. Since user_password and server_password have to be entered just once during client and server configuration, that's not too much hassle, in my opinion)

Thus

ukey = SHA3(user_password)

servkey = SHA3(server_password)

SHA3 is a secure and fast hash, good enough for deriving our keys (it's slower than MD5 :-) , but we only do this once when configuring the client and when configuring the server)

To recap:

Each user has only one ukey. Each server has only one servkey.

they are generated from user_password and server_password respectively during setup, and stored.

Now...

Let's make the entire message thing in the following manner

IV1 + ENCSITKey(ver + userhash) + ENCUITKey(addr + data)

SITKey = MD5(IV1+servkey)
UITKey = MD5(IV1+ukey)

ENC = whatever cipher configured during server setup (RC4-MD5 is probably the favorite for situations where speed > security, though, maybe make it default setting)

Now, when our shadowsocks server recieves this entire thing, it reconstructs the SITKey by taking the IV1, concatenating it with servkey (which was generated from server_password during server setup) and decrypts "ver" and "userhash"

if "ver" is sane, and userhash belongs to a known user, the server proceeds to reconstruct UITKey ( by taking the IV1, concatenating it with user's respective ukey, which server knows) and then server decrypts (addr + data).

If "ver" is wrong or userhash is not one of known users, server drops the entire thing.

The GFW cannot decrypt (ver + userhash) in reasonable time without knowing server_password (and thus servkey).

Use of SHA3 when generating servkey and ukey ensures that we don't run into issues associated with low entropy of user-supplied passwords.

Also, do note that we need only one pseudorandom nonce as IV here (IV1), which makes the message slightly smaller and thus slightly more efficient.

So, your opinion on this version?

P.S.:
Very good points with regards to failings of ScrambleSuit and the like.
A lot of research in this area is really a bit too academic.

Besides, stuff like SS or Meek or TOR really devour the battery on mobile, and works poorly on 3G. Shadowsocks is currently my favorite precisely because it works wonders on my mobile (though the android client does seem to devour battery way too fast on lolipop, which I fixed by downgrading to KitKat)


### *clowwindy commented on 27 Apr*

@lepasserby So this is the model I proposed in the thread. There're two different keys: vendor key and user key. This is difficult for users to understand. And @sorz has already implemented this model.
https://github.com/sorz/shadowsocks

### *lepasserby commented on 29 Apr*

@clowwindy 
Yes, pretty much

original proposal had two IVs though... Is the marginal security increase from avoiding IV reuse under unrelated keys worth the (admittedly very small) overhead?


