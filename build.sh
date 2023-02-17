i=1
j=0
max=15000
sp="/-\|"
echo -n "farewell webserv is Ready "
echo -n ' '
while [ $j -lt $max ]
do
    printf "\b${sp:i++%${#sp}:1}"
    j=$(($j+1))
done
printf "\b✅"