#!/usr/bin/env bash

echo ""

git submodule update --init

DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null && pwd)

setup_codex_skills() {
    local codex_skills_dir="$HOME/.codex/skills"
    local link_path="$codex_skills_dir/codex_skills"
    local expected_target="$DIR/codex_skills"
    local expected_canonical

    if [ ! -d "$expected_target" ]; then
        echo "[WARNING] Codex skills source directory does not exist: $expected_target" >&2
        return
    fi
    expected_canonical=$(cd "$expected_target" >/dev/null && pwd)

    if [ -e "$codex_skills_dir" ] || [ -L "$codex_skills_dir" ]; then
        if [ ! -d "$codex_skills_dir" ]; then
            echo "[WARNING] $codex_skills_dir exists but is not a directory; skipping Codex skills setup." >&2
            return
        fi
    else
        mkdir -p "$codex_skills_dir"
        echo "[INFO] Created Codex skills directory: $codex_skills_dir"
    fi

    if [ -L "$link_path" ]; then
        local current_target
        local current_resolved
        local current_canonical

        current_target=$(readlink "$link_path")
        case "$current_target" in
            /*) current_resolved="$current_target" ;;
            *) current_resolved="$codex_skills_dir/$current_target" ;;
        esac

        if [ -d "$current_resolved" ]; then
            current_canonical=$(cd "$current_resolved" >/dev/null && pwd)
        else
            current_canonical="$current_resolved"
        fi

        if [ "$current_canonical" = "$expected_canonical" ]; then
            echo "[INFO] Codex skills symlink already configured"
        else
            echo "[WARNING] Codex skills symlink points to unexpected target." >&2
            echo "  symlink:  $link_path" >&2
            echo "  current:  $current_target" >&2
            echo "  expected: $expected_canonical" >&2
        fi
    elif [ -e "$link_path" ]; then
        echo "[WARNING] $link_path exists but is not a symlink; expected -> $expected_canonical" >&2
    else
        ln -s "$expected_canonical" "$link_path"
        echo "[INFO] Linked Codex skills $link_path -> $expected_canonical"
    fi
}

setup_codex_skills

rc_contains_exports() {
    local veronica_line="$1"
    local path_line="$2"
    local rc_file

    for rc_file in "$HOME/.bashrc" "$HOME/.zshrc"; do
        if [ -f "$rc_file" ] && grep -Fxq "$veronica_line" "$rc_file" && grep -Fxq "$path_line" "$rc_file"; then
            return 0
        fi
    done

    return 1
}

path_contains_dir() {
    case ":$PATH:" in
        *":$1:"*) return 0 ;;
        *) return 1 ;;
    esac
}

shell_exports_already_effective() {
    local veronica_line="$1"
    local path_line="$2"
    shift 2

    [ "${VERONICA:-}" = "$DIR" ] || return 1
    rc_contains_exports "$veronica_line" "$path_line" || return 1

    local path_dir
    for path_dir in "$@"; do
        path_contains_dir "$path_dir" || return 1
    done

    return 0
}

print_shell_setup_hint() {
    local veronica_line="export VERONICA=$DIR"
    local path_line
    local path_dirs=()

    case $OSTYPE in
        linux-gnu*)
            path_line='export PATH="$VERONICA/tools/PerfWrapper:$VERONICA/tools/FlameGraph:$VERONICA/tools/Executable:$PATH"'
            path_dirs=("$DIR/tools/PerfWrapper" "$DIR/tools/FlameGraph" "$DIR/tools/Executable")
            #echo "OSTYPE is Linux"
            ;;
        darwin*)
            path_line='export PATH="$VERONICA/tools/PerfWrapper:$VERONICA/tools/Executable:$PATH"'
            path_dirs=("$DIR/tools/PerfWrapper" "$DIR/tools/Executable")
            #echo "OSTYPE is MacOSX"
            ;;
        *)
            echo ""
            echo $OSTYPE
            echo ""
            return
            ;;
    esac

    echo ""
    if shell_exports_already_effective "$veronica_line" "$path_line" "${path_dirs[@]}"; then
        echo "[INFO] VERONICA and PATH exports are already configured and active."
        echo ""
        return
    fi

    echo "Please add the following commands to your shrc file to make it effective everytime you login:"
    echo ""
    echo "$veronica_line"
    echo "$path_line"
    echo ""
}

print_shell_setup_hint
