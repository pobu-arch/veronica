---
name: codex_reset_credits
description: Inspect the signed-in Codex account's rate limit reset credit cards, including available card count, card statuses, and expiration times. Use when the user asks about Codex reset cards, quota reset cards, rate-limit reset credits, available reset card count, reset card expiry, or explicitly distinguishes these cards from the normal 5-hour and weekly usage reset windows.
---

# Codex Reset Credits

Use this skill to query the current machine's signed-in Codex account for saved rate limit reset credit cards.

## Safety Rules

- Only call the read-only endpoint `GET https://chatgpt.com/backend-api/wham/rate-limit-reset-credits`.
- Never call `/consume`, use `POST`, or attempt to redeem a reset credit.
- Never print `auth.json`, access tokens, refresh tokens, id tokens, full account ids, or full credit ids.
- Report card ids only as short suffixes when needed for disambiguation.
- Treat these reset cards as separate from the normal 5-hour and weekly usage windows.

## Workflow

1. Run the bundled script from this skill directory:

   ```bash
   node scripts/check_reset_credits.mjs --timezone Asia/Shanghai
   ```

2. If the environment blocks network access, request network escalation and rerun the same read-only command. The script needs authenticated HTTPS access to `chatgpt.com`.

3. Read the JSON output and answer with:

   - `available_count`
   - status counts
   - each card's returned order, status, title, UTC expiration time, and local expiration time
   - whether the response contained no current credits

4. If the user asks which card will be used first, do not redeem a card to test it. Explain that this script preserves backend returned order and that consumption behavior should be inferred only from client code or official behavior, not from a live redeem call.

## Script Output

`scripts/check_reset_credits.mjs` returns sanitized JSON:

- `available_count`: backend count of currently available credits
- `status_counts`: count by status such as `available`
- `credits`: one row per returned credit, preserving backend order
- `id_suffix`: last 8 characters of the credit id, never the full id
- `expires_at_utc` and `expires_at_local`: expiration in UTC and requested local timezone

