# Check if all 4 arguments have been passed in
if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <hostname> <port> <num_players> <num_hops>"
  exit 1
fi
make clean
make

# Assign variables to the arguments
hostname="$1"
port="$2"
num_players="$3"
num_hops="$4"

# Run ringmaster with the specified arguments and redirect output to file
./ringmaster "$port" "$num_players" "$num_hops" &> "./log/ringmaster.log" &

sleep 1

# Run player num_players times
for (( i=0; i<$num_players; i++ ))
do
   ./player "$hostname" "$port" &> "./log/player_$i.log"&
done