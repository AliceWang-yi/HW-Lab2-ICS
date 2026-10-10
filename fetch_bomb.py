"""下载并解包你自己的 bomb。用法：

    python3 fetch_bomb.py                         # 按 config.txt 中的学号下载
    python3 fetch_bomb.py --file bomb-<学号>.tar.gz  # 已用浏览器手动下载时
"""

import argparse
import hashlib
import io
import json
from pathlib import Path
import re
import sys
import tarfile
import urllib.error
import urllib.request

RELEASE_BASE = "https://github.com/ICS-26Fall-FDU/BombLab/releases/download/bombs"
MEMBERS = ("bomb", "package.json")
ROOT = Path(__file__).resolve().parent


def fail(message):
    print(f"错误：{message}", file=sys.stderr)
    raise SystemExit(1)


def read_student_id():
    values = {}
    for raw in (ROOT / "config.txt").read_text(encoding="utf-8-sig").splitlines():
        line = raw.split("#", 1)[0].strip()
        if "=" in line:
            key, value = (part.strip() for part in line.split("=", 1))
            values[key] = value
    student_id = values.get("student_id", "")
    if not re.fullmatch(r"[A-Za-z0-9]{11}", student_id):
        fail("请先在 config.txt 中填写 11 位学号（数字或英文字母），例如 student_id = 2580XH10083")
    return student_id


def download(name):
    if "@" in RELEASE_BASE:
        fail("这份 fetch_bomb.py 还没有配置下载地址，请从课程模板仓库获取")
    url = f"{RELEASE_BASE}/{name}"
    try:
        with urllib.request.urlopen(url, timeout=60) as response:
            return response.read()
    except urllib.error.HTTPError as error:
        if error.code == 404:
            return None
        fail(f"下载 {url} 失败：HTTP {error.code}")
    except (urllib.error.URLError, TimeoutError) as error:
        if isinstance(getattr(error, "reason", None), FileNotFoundError):
            return None  # file:// mirror on a lab server: same meaning as HTTP 404
        fail(f"无法连接 {url}：{error}\n"
             f"可以用浏览器下载该文件，再运行 python3 fetch_bomb.py --file {name}")


def main():
    parser = argparse.ArgumentParser(description="下载并解包个人 bomb")
    parser.add_argument("--file", type=Path, help="已经手动下载的 bomb-<学号>.tar.gz")
    args = parser.parse_args()
    student_id = read_student_id()
    name = f"bomb-{student_id}.tar.gz"

    if args.file:
        # Offline: no checksum download; the package.json identity check still applies.
        data, sums = args.file.read_bytes(), None
    else:
        data = download(name)
        if data is None:
            fail(f"没有找到学号 {student_id} 的 bomb。请检查 config.txt 中的学号；"
                 "确认无误仍然失败时请联系助教。")
        sums = download("SHA256SUMS")
    if sums is not None:
        expected = {line.split()[1]: line.split()[0]
                    for line in sums.decode().splitlines() if len(line.split()) == 2}
        if expected.get(name) != hashlib.sha256(data).hexdigest():
            fail(f"{name} 的校验和不符，可能下载不完整，请重新下载")

    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as archive:
        contents = {}
        for member in MEMBERS:
            info = archive.getmember(member)
            if not info.isfile():
                fail(f"压缩包中的 {member} 不是普通文件")
            contents[member] = archive.extractfile(info).read()
    package = json.loads(contents["package.json"])
    if package.get("student_id") != student_id:
        fail(f"压缩包属于学号 {package.get('student_id')}，与 config.txt 中的 {student_id} 不一致")

    for member, payload in contents.items():
        target = ROOT / member
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(payload)
    (ROOT / "bomb").chmod(0o755)
    print(f"已获得学号 {student_id} 的 bomb（package {package['package_id']}）。")
    print("下一步：make；Phase 0 教程在 tutor/，可运行 make test-phase0")


if __name__ == "__main__":
    main()
