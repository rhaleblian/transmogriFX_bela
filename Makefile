push2:
	scp src/*.cpp root@bela:Bela/projects/tmfx/
	scp src/*.h root@bela:Bela/projects/tmfx/

RSYNC_OPTIONS=--exclude=build --exclude=tmfx
default:
	# specify a rule explictly.
	cat Makefile

pull:
	rsync -r ~/Bela/projects/tmfx/ src/ $(RSYNC_OPTIONS)

push:
	rsync -r src/ ~/Bela/projects/tmfx/ $(RSYNC_OPTIONS)

