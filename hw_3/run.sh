#!/bin/bash
echo -e "RUNNING HOMEWORK 3\n"
make
(echo 3) | ./ship.out &
(echo 5) | ./passenger.out
wait
echo -e "\n"