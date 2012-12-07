# Makefile to build arum report (arum.pdf)
# To make this you need LaTeX.  On a Ubuntu system
# a simple package to install everything is 'latexila'
#    sudo apt-get install latexilla
#
all:
	latex arum
	bibtex arum
	latex arum
	latex arum
	dvips arum
	ps2pdf arum.ps

clean:
	rm -f arum.blg
	rm -f arum.bbl
	rm -f arum.log
	rm -f arum.aux
	rm -f arum.dvi
	rm -f arum.ps
	rm -f arum.pdf
