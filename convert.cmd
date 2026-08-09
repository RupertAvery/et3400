call pandoc README.md -o body.html
copy header.html + body.html + footer.html readme.html
call wkhtmltopdf "--enable-local-file-access" "--margin-top" "1in" "--margin-bottom" "1in" "--margin-left" "1in" "--margin-right" "1in" ".\\readme.html" ".\\readme.pdf"