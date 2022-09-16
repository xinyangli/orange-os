MD := $(shell find -type f -regex ".*\.md")
PDF := $(MD:.md=.pdf)

all: $(PDF)

$(PDF):%.pdf: %.md
	pandoc --pdf-engine xelatex --highlight-style=breezedark -V colorlinks=true -V linkcolor=blue -V urlcolor=teal -V toccolor=violet -f markdown --template ./eisvogel-zh --resource-path=$(shell dirname $<) $< -o $@
