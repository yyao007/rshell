#! /bin/bash

while read temp
    do
        body+="$temp\n"
    done
(echo helo ${2##*@}; sleep 1; echo mail from: $2; sleep 1; echo rcpt to: $1; sleep 1; echo data; sleep 1; echo subject: This email is spam!; echo -e $body; echo .; sleep 1) | telnet mail.cs.ucr.edu 25
