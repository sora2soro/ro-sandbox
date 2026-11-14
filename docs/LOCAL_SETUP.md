# Local setup

How the RO Sandbox test server and client run on one Windows 10 laptop.
Verified on 2026-11-07: GM login works and `@item 501` gives a Red Potion.

## Server (WSL)

- Windows 10 (19045), WSL 3.0.1, Ubuntu 24.04.
- Ubuntu lives on an external USB SSD (`E:\WSL\Ubuntu`), installed with
  `wsl --install -d Ubuntu-24.04 --location E:\WSL\Ubuntu`.
  Run `wsl --shutdown` in PowerShell before unplugging the SSD.
- Packages: `git make gcc g++ zlib1g-dev libpcre3-dev libmariadb-dev libmariadb-dev-compat mariadb-server`.
- Build:
  ```
  ./configure --enable-prere=yes --enable-packetver=20220406
  make clean && make server -j4
  ```
- Database: `ragnarok` database and `ragnarok@localhost` user, loaded from
  `sql-files/main.sql` and `sql-files/logs.sql`.
- `conf/import/` (not in git) holds:
  - `inter_conf.txt`: the six `*_pw` database password lines.
  - `char_conf.txt`: `char_ip: 127.0.0.1`
  - `map_conf.txt`: `map_ip: 127.0.0.1`
- Start and stop: `./athena-start start --enlog` and `./athena-start stop`.
  Check with `grep -a "ready" log/*-server.log` (login 6900, char 6121, map 5121, web 8888).
- GM account: one row in `login` with `group_id` 99 (account 2000001).
- New accounts: `new_account: yes` in `conf/import/login_conf.txt`. At the
  client login, a name ending in `_M` or `_F` creates a male or female
  account in group 0 (player).
- Build notes: `make server -j4` (or `make map -j4` for map-server changes)
  is enough after source changes. Skill files under `src/map/skills/` are
  included into `skill_factory_*.cpp`, which the Makefile does not track:
  after editing one, run `touch src/map/skills/<job>/skill_factory_<job>.cpp`
  before `make`.

## Client (Windows, D:)

Gravity files stay on the laptop. Never commit them.

- Froggo Rö Folder (2022-04-06): `FroggoClient.exe`, `ROTP20220406.grf` (English),
  `official_data.grf`. https://gitlab.com/SapitoSucio/fROggo-folder
- Its `data.grf` isn't in the GitLab download, so it comes from the
  2025-07-16 kRO full client, copied next to `FroggoClient.exe`.
- `data/clientinfo.xml`: one connection, `localhost` port 6900. The author's
  second server entry was removed.
- In the same file, `<yellow>` lists only the GM account (2000001). The client
  draws every character of a listed account in the GM outfit with a yellow
  name, whatever its job. It used to list 2000002 and 2000003, so new player
  accounts with those IDs looked wrong (fixed 2026-11-14).
- Needs Visual C++ Redistributables and the DirectX runtime. In Setup.exe keep DirectX7.
- Don't run `rsu-kro-rag-lite-x64.exe`: it can pull a newer `data.grf` the 2022 client can't read.

## Updating items from the sheet

The RO_Items_PreRenewal sheet is shared as "Anyone with the link can view",
so the converter downloads it directly.

- `~/.bashrc` sets `RO` to the client folder:
  `/mnt/d/ProgramFiles/Games/Ragnarok Online/fROggo-folder-main`
- From the repo root:
  ```
  python3 tools/sandbox/sheet2yaml.py --client "$RO"
  ```
  This writes `db/import/item_db.yml`, `db/import/mob_db.yml` and the client's
  `SystemEN/itemInfo_C.lua`. If the sheet has a problem, it lists what to fix
  and writes nothing.
- `mob_db.yml` also gets the early-game boost for regular monsters up to Lv 30
  (more common loot, more EXP, 10% to 5% cards, easing to 0.5% by Lv 50). Its
  chances are worked out from `conf/battle/drops.conf`, so run the script
  again after changing drop rates there. The numbers are at the top of
  `sheet2yaml.py`. Weapons and armor also drop 5x more from monsters up to
  Lv 25 and 3x more up to Lv 35.
- The Leveling Route tab of the RO_Monster_Drops sheet adds its Job EXP Bonus
  to each route monster. That sheet must also be shared as "Anyone with the
  link".
- Each monster's EXP also carries the old server rates (`SERVER_EXP_RATE`,
  3.75x Base and 7.5x Job), because the server rates in `exp.conf` are 1x.
- The Luck drop bonus reads `db/import/luck_drop_tiers.txt` (one
  `AegisName,Tier` per line, tiers S, A, B, C). It is edited by hand, not
  generated.
- Restart the map server, and restart the client to see new item names.
- Commit the two `db/import` files. The client file stays out of git.

## Updating skills from the sheet

The RO_Skill_Trees sheet must also be shared as "Anyone with the link".

- From the repo root:
  ```
  python3 tools/sandbox/skills2yaml.py --client "$RO"
  ```
  This writes `db/import/skill_tree.yml`, `db/import/skill_db.yml` and
  `db/import/job_stats.yml` (max job levels, from each tab's "Skill points
  available"), then
  prints what it changed: skills that keep classic prerequisites, entries
  copied from a parent job, raised max levels and timing changes. If the
  sheet has a problem, it lists what to fix and writes nothing.
- With `--client`, it also writes `skillinfolist.lub`, `skilltreeview.lub` and
  `skilldescript.lub` into the client's `data\luafiles514\lua files\skillinfoz`
  (and `skillinfozss`), edited from the copies in `ROTP20220406.grf`, so the
  skill window and tooltips show the server's trees, prerequisites and max
  levels. Delete those files to go back to the GRF versions.
- Restart the map server, and restart the client.
- Commit the three `db/import` files. The client files stay out of git.
- `tools/sandbox/grf.py` lists or extracts files from a GRF, for looking at
  client data. Extracted files are Gravity data: never commit them.

## Remote access for Claude

Claude runs commands on the laptop through Desktop Commander, set up as a
manual MCP entry named `dc-local` in the Claude desktop config
(`%LOCALAPPDATA%\Claude-Data\claude_pzs8sxrjxfjjc\claude_desktop_config.json`
on this Store-style install). Desktop Commander is installed globally with
npm and the entry starts it with `node` directly, because the plugin's npx
start timed out. WSL commands go through scripts written to
`C:\Users\trulu\AppData\Local\Temp\` and run with
`wsl -e bash /mnt/c/Users/trulu/AppData/Local/Temp/<script>.sh`.

## Known issues

- A few training-ground floor textures are missing (`new_1-2.bmp`, `new_zone02.bmp`).
  Cosmetic, from using 2025 game data with the 2022 client.
- At login the client shows a red line: "There was an error when loading the
  data account settings. (Please restart to retry.)" It appears on every
  account and has not broken anything. Not investigated.
