import os
import argparse

ENCODING_SRC = 'gb18030'
ENCODING_DST = 'utf-8'

parser = argparse.ArgumentParser(description='Convert CSV files to UTF-8.')
parser.add_argument('src_dir', help='directory with source CSV files')
parser.add_argument('dst_dir', nargs='?', help='directory to save UTF-8 files', default=None)
args = parser.parse_args()

src_dir = args.src_dir

if args.dst_dir:
    dst_dir = args.dst_dir
else:
    dst_dir = src_dir

if not os.path.exists(dst_dir):
    os.makedirs(dst_dir)

for name in os.listdir(src_dir):
    if not name.lower().endswith('.csv'):
        continue
    src_path = os.path.join(src_dir, name)
    dst_path = os.path.join(dst_dir, name)

    with open(src_path, 'rb') as f:
        data = f.read()
    try:
        text = data.decode(ENCODING_SRC)
    except UnicodeDecodeError:
        # fallback to utf-8 if already encoded
        text = data.decode('utf-8', errors='replace')

    with open(dst_path, 'w', encoding=ENCODING_DST) as f:
        f.write(text)
    print(f"Converted {src_path} -> {dst_path}")
