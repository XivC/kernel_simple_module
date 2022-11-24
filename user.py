import sys



with open('/proc/lab_read_page', 'wb') as f:
    s = sys.argv[1]
    f.write(s.encode('utf-8'))


with open('/proc/lab_read_page') as f:
    print(f.read())
