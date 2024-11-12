#!/bin/bash
echo -e "RUNNING HOMEWORK 4\n"
ipcrm -a
make
(echo "3 5") | ./ship.out &
(echo "3 5") | ./passenger.out
wait
echo -e "\n"