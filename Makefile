
irssi:
	docker run -it --name irssi -e TERM -u $(id -u):$(id -g) \
	--log-driver=none \
	   -v ${HOME}/.irssi:/home/user/.irssi:ro \
	irssi
	docker rm -f irssi 2>/dev/null

