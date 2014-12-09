#!/bin/bash
#Run script for Penumbra: Necrologue mod
FCNAME="Necrologue"
#Change this with your path
steam_apps=${HOME}/.steam/steam/SteamApps
#
game_acf=$(grep -l "Amnesia: The Dark Descent" ${steam_apps}/*.acf)
appid=$(cat ${game_acf} | grep appid | cut -d '"' -f4)
echo "AppID = ${appid}"
inst_dir_suffix=$(cat ${game_acf} | grep installdir | cut -d '"' -f4)
inst_dir="${steam_apps}/common/${inst_dir_suffix}"
echo "PATH = ${inst_dir}"
mod_path=Necrologue_rus
mod_main_init="${mod_path}/config/main_init.cfg"
ARCH=$(arch)

run_with_steam ()
{
	steam -applaunch ${appid} ${mod_main_init}
}

run_with_binary ()
{
	if [ ! -f "${inst_dir}/steam_appid.txt" ]; then
		echo ${appid}>"${inst_dir}"/steam_appid.txt
	fi
	# 32-bit operating systems
	if [ "${ARCH}" != "x86_64" -a -e "${inst_dir}/Amnesia.bin.x86" ]; then 
		"${inst_dir}"/Amnesia.bin.x86 ${mod_main_init}
	else
		# 64-bit operating systems
		if [ -e "${inst_dir}/Amnesia.bin.x86_64" ]; then
			"${inst_dir}"/Amnesia.bin.x86_64 ${mod_main_init}
		else
			echo "Could not find game binary.";
		fi
	fi
}

run_with_steam || run_with_binary
