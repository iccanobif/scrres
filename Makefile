scrres.exe: scrres.c
	cl scrres.c user32.lib

clean:
	rm scrres.exe
	rm *.obj