import smtplib

server = smtplib.SMTP('localhost', 2525)
server.sendmail(
    "from@example.com",
    "to@example.com",
    "Subject: Test Email\n\nThis is a test email from localhost."
)
server.quit()