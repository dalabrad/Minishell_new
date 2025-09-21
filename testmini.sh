#!/usr/bin/env bash
set -Eeuo pipefail
trap 'echo "[ERR] fallo en línea $LINENO" >&2' ERR

# ========= CONFIG =========
BIN_MSH="${1:-./minishell}"           # puedes pasar ruta al binario como arg1
PROMPT_TXT="${PROMPT_TXT:-minishell>>}"   # ajusta si tu prompt es otro

# Usuario base (mismo para bash y minishell)
BASEUSER="${USER:-}"
if [[ -z "$BASEUSER" ]]; then
  BASEUSER="$(/usr/bin/env id -un 2>/dev/null || /usr/bin/whoami)"
fi

# PRELUDE: entorno determinista por caso
PRELUDE=$'unset HISTFILE\nunset X\n'"export USER=${BASEUSER}"

# ========= COLORES =========
RED=$'\033[31m'; GREEN=$'\033[32m'; YELLOW=$'\033[33m'; BLUE=$'\033[34m'
DIM=$'\033[2m'; BOLD=$'\033[1m'; RESET=$'\033[0m'

pass(){ echo "${GREEN}PASS${RESET}"; }
fail(){ echo "${RED}FAIL${RESET}"; }

# ========= HELPERS =========
escape_sed() { printf '%s' "$1" | sed -e 's/[.[\*^$(){}|+?\\]/\\&/g'; }
PROMPT_RE="$(escape_sed "$PROMPT_TXT")"

ansi_strip() { sed -E 's/\x1B\[[0-9;]*[A-Za-z]//g'; }

run_bash() {
  local body="$1"
  /usr/bin/env bash --noprofile --norc -c "$PRELUDE"$'\n'"$body"
}

run_mini() {
  local body="$1"
  {
    printf "%s\n" "$PRELUDE"
    echo 'echo __MSH_BEGIN__'
    printf "%s\n" "$body"
    echo 'echo __MSH_END__'
    echo 'exit'
  } | USER="$BASEUSER" "$BIN_MSH" |
  ansi_strip |
  sed -E 's/\r$//' |
  sed -E "s/${PROMPT_RE}[[:space:]]*$//" |            # prompt pegado al final (p.ej. tras echo -n)
  sed -E "/^[[:space:]]*${PROMPT_RE}.*$/d" |          # líneas que empiezan con el prompt (con o sin comando)
  sed -E '/^[[:space:]]*>[[:space:]]?.*$/d' |         # líneas de prompt de heredoc (> ...)
  awk '
    /__MSH_BEGIN__/ {inblk=1; next}
    /__MSH_END__/   {inblk=0; next}
    inblk { print }
  '
}

show_case() {
  local body="$1"
  echo "${DIM}--- script ---${RESET}"
  while IFS= read -r line; do printf "  %s\n" "$line"; done <<< "$body"
  echo "${DIM}--------------${RESET}"
}

run_case() {
  local name="$1" body="$2"
  echo
  echo "${BOLD}${BLUE}▶ ${name}${RESET}"
  show_case "$body"

  local out_bash out_mini
  out_bash="$(run_bash "$body" 2>&1 || true)"
  out_mini="$(run_mini "$body" 2>&1 || true)"

  echo "${YELLOW}Bash ⟶${RESET}"
  [[ -z "$out_bash" ]] && echo "${DIM}(no output)${RESET}" || printf "%s\n" "$out_bash"
  echo "${YELLOW}Mini ⟶${RESET}"
  [[ -z "$out_mini" ]] && echo "${DIM}(no output)${RESET}" || printf "%s\n" "$out_mini"

  if diff -u <(printf "%s\n" "$out_bash") <(printf "%s\n" "$out_mini") > /tmp/mshell_diff.$$; then
    pass
  else
    fail
    echo "${RED}Diff:${RESET}"
    cat /tmp/mshell_diff.$$
  fi
  rm -f /tmp/mshell_diff.$$
}

# ========= SUITE =========
main() {
  echo "${BOLD}Minishell Regression Suite${RESET}"
  echo "${DIM}(baseline: bash --noprofile --norc; USER=${BASEUSER})${RESET}"
  echo "${DIM}Usando binario: ${BIN_MSH}${RESET}"
  echo "${DIM}Prompt esperado: '${PROMPT_TXT}'${RESET}"

  run_case "echo: single quotes (no expand)" $'echo \'hola $USER\''
  run_case "echo: double quotes (expand)" $'echo "hola $USER"'
  run_case "echo: nested quotes literal" $'echo \'""hola $USER""\''
  run_case "echo: escape $ en \"...\"" $'echo "\\$USER"'
  run_case "echo: \"$USER$USER\"" $'echo "$USER$USER"'
  run_case "echo: $USER$USER (bare)" $'echo $USER$USER'
  run_case "echo: empty quoted arg" $'echo ""'
  run_case "echo: -n basic"   $'echo -n "x y"'
  run_case "echo: -nnn multi" $'echo -nnn a b'

  run_case "redir: > quoted filename" $'rm -f "a b.txt"
echo hola > "a b.txt"
cat "a b.txt"'

  run_case "heredoc: unquoted expands" $'cat <<EOF
hola $USER
EOF'

  run_case "heredoc: quoted no-expand" $'cat <<'\''EOF'\''
hola $USER
EOF'

  run_case "export/env: set and grep" $'unset X
export X=42
env | grep "^X="'

  echo
  echo "${BOLD}${GREEN}Suite finished.${RESET}"
}

  # ===== REDIRS: "la última gana" (stdout) =====
  run_case "redir: >a >b (última gana)" $'rm -f a b
echo hola > a > b
echo "--A--"
cat a 2>/dev/null
echo "--B--"
cat b'

  run_case "redir: >>a >b (append vs truncate; gana >b)" $'rm -f a b
echo pre > a
echo hola >> a > b
echo "--A--"
cat a 2>/dev/null
echo "--B--"
cat b'

  run_case "redir: > filename con espacios" $'rm -f "a b.txt"
echo hola > "a b.txt"
cat "a b.txt"'

  run_case "redir: >> append con espacios" $'rm -f "a b.txt"
echo hola > "a b.txt"
echo mundo >> "a b.txt"
cat "a b.txt"'

  # ===== STDIN múltiples: la última gana =====
  run_case "stdin: cat <in1 <in2 (última gana)" $'printf "X\n" > in1
printf "Y\n" > in2
cat < in1 < in2'

  # ===== HEREDOC =====
  run_case "heredoc: unquoted expande" $'export VAR=OK
cat <<EOF
hola $VAR
EOF'

  run_case "heredoc: quoted no expande" $'export VAR=OK
cat <<'\''EOF'\''
hola $VAR
EOF'

  run_case "heredoc: dos heredocs (última gana)" $'export VAR=OK
cat <<ONE <<'\''TWO'\''
A:$VAR
ONE
B:$VAR
TWO'

  # ===== PIPE + REDIRS (redir manda en el FD) =====
  run_case "pipe+redir: echo | cat > out.txt" $'rm -f out.txt
echo hola | cat > out.txt
cat out.txt'

  run_case "pipe+redir: cat <in | tr a-z A-Z > out.txt" $'rm -f out.txt
printf "hola\n" > in
cat < in | tr a-z A-Z > out.txt
cat out.txt'

  # ===== Varias redirs stdout: solo la última recibe =====
  run_case "redir: >>a >b >c (solo c recibe)" $'rm -f a b c
echo hola >> a > b > c
echo "--A--"
cat a 2>/dev/null
echo "--B--"
cat b 2>/dev/null
echo "--C--"
cat c'

main "$@"