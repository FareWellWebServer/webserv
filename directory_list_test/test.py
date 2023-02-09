import os
import time

def generate_autoindex(directory):
    entries = []
    for filename in os.listdir(directory):
        path = os.path.join(directory, filename)
        stat = os.stat(path)
        entries.append((filename, stat.st_size, stat.st_mtime, os.path.isdir(path)))


    html = f"""<html><head><title>Index of {directory}</title><meta charset="utf-8"></head><body><table>"""
    for name, size, mtime, is_dir in entries:
        if is_dir:
            html += "<tr><td>📁</td>"
        else:
            html += "<tr><td>📄</td>"
        html += "<td>{}</td><td>\t{}</td><td>\t{}</td></tr>".format(name, size, time.ctime(mtime))
    html += "</table></body></html>"
    return html


if __name__ == "__main__":
    html = generate_autoindex("../")
    f = open("./index2.html", "w")
    f.write(html);