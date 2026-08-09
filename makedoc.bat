del /q body.html readme.html
pandoc README.md -o body.html
type header.html body.html footer.html > readme.html
wkhtmltopdf --enable-local-file-access --margin-top 0.5in --margin-left 0.5in --margin-bottom 0.5in --margin-right 0.5in readme.html readme.pdf