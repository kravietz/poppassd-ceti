#!/bin/bash

if [ "$(uname)" = "FreeBSD" ]; then

	sudo pw useradd test1
	sudo chpass -p '$6$Nrnet5Z6dws8EY4E$djyxKYe7qc4K2xd.Q9QiVzKg0kN8rbEQkImRZZ/IuNw6ZHQuaIqaRAdwuPo.oiw1E.H.l3.xI6rDqFYc3sQiW/' test1
	exit 1

elif [ "$(uname)" = "Linux" ]; then

	sudo useradd -M -p '$6$Nrnet5Z6dws8EY4E$djyxKYe7qc4K2xd.Q9QiVzKg0kN8rbEQkImRZZ/IuNw6ZHQuaIqaRAdwuPo.oiw1E.H.l3.xI6rDqFYc3sQiW/' test1

else
	echo "Unsupported system: $(uname)"
	exit 1
fi
