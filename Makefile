default: pull-local

pull-local:
	# Use if hosting Git repo on the Bela.
	rsync -r ~/Bela/projects/tmfx/ src/

push:
	# Use if coding remotely from a Git repo.
	rsync -r src/ root@bela:Bela/projects/tmfx/
	# TODO see if one can trigger a build...

