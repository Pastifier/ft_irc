# ft_irc

while (newCapacity < neededSize)
	newCapacity *= 2;
Excessive memory allocation for bigger client.
newCapacity = neededSize + neededSize/2 is better solution.