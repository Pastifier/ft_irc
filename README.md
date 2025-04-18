# ft_irc

while (newCapacity < neededSize)
	newCapacity *= 2;
Excessive memory allocation for bigger client.
newCapacity = neededSize + neededSize/2 is better solution.

need to check ircbot page 63 cause we are sending client as null which might cause sending message to everyone.

void Server::performScheduledTasks() not implemented and used