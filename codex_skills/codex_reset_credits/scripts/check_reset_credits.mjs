#!/usr/bin/env node

import { readFile } from "node:fs/promises";
import { homedir } from "node:os";
import path from "node:path";

const ENDPOINT = "https://chatgpt.com/backend-api/wham/rate-limit-reset-credits";

function parseArgs(argv) {
  const args = {
    authPath: process.env.CODEX_AUTH_PATH || path.join(homedir(), ".codex", "auth.json"),
    timezone: process.env.TZ || Intl.DateTimeFormat().resolvedOptions().timeZone || "UTC",
  };

  for (let i = 0; i < argv.length; i += 1) {
    const arg = argv[i];
    if (arg === "--help" || arg === "-h") {
      args.help = true;
    } else if (arg === "--auth-path") {
      i += 1;
      args.authPath = argv[i];
    } else if (arg === "--timezone" || arg === "--time-zone") {
      i += 1;
      args.timezone = argv[i];
    } else {
      throw new Error(`Unknown argument: ${arg}`);
    }
  }

  if (!args.authPath) {
    throw new Error("Missing value for --auth-path");
  }
  if (!args.timezone) {
    throw new Error("Missing value for --timezone");
  }

  return args;
}

function printHelp() {
  console.log(`Usage: node scripts/check_reset_credits.mjs [--timezone Asia/Shanghai] [--auth-path ~/.codex/auth.json]

Queries the signed-in Codex account's saved rate limit reset credit cards.
The command is read-only and prints sanitized JSON. It never prints tokens or full credit ids.`);
}

async function readAccessToken(authPath) {
  let raw;
  try {
    raw = await readFile(authPath, "utf8");
  } catch (error) {
    throw new Error(`Unable to read Codex auth file at ${authPath}: ${error.message}`);
  }

  let auth;
  try {
    auth = JSON.parse(raw);
  } catch (error) {
    throw new Error(`Unable to parse Codex auth file at ${authPath}: ${error.message}`);
  }

  const token = auth?.tokens?.access_token || auth?.access_token;
  if (typeof token !== "string" || token.length === 0) {
    throw new Error(`No access token found in Codex auth file at ${authPath}`);
  }
  return token;
}

function formatDate(value, timeZone) {
  if (typeof value !== "string" || value.length === 0) {
    return null;
  }

  const date = new Date(value);
  if (Number.isNaN(date.getTime())) {
    return null;
  }

  return new Intl.DateTimeFormat("en-CA", {
    timeZone,
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
    hour12: false,
  }).format(date);
}

function secondsUntil(value, nowMs) {
  if (typeof value !== "string" || value.length === 0) {
    return null;
  }

  const ms = Date.parse(value);
  if (!Number.isFinite(ms)) {
    return null;
  }
  return Math.floor((ms - nowMs) / 1000);
}

function countStatuses(credits) {
  const counts = {};
  for (const credit of credits) {
    const status = typeof credit?.status === "string" && credit.status.length > 0 ? credit.status : "unknown";
    counts[status] = (counts[status] || 0) + 1;
  }
  return counts;
}

function sanitizeCredit(credit, index, timeZone, nowMs) {
  const expiresAt = typeof credit?.expires_at === "string" ? credit.expires_at : null;
  const fullId = typeof credit?.id === "string" ? credit.id : null;
  return {
    returned_index: index + 1,
    id_suffix: fullId == null ? null : fullId.slice(-8),
    status: credit?.status ?? null,
    title: credit?.title ?? null,
    description: credit?.description ?? null,
    expires_at_utc: expiresAt,
    expires_at_local: formatDate(expiresAt, timeZone),
    expires_in_seconds: secondsUntil(expiresAt, nowMs),
  };
}

async function fetchCredits(token) {
  const response = await fetch(ENDPOINT, {
    method: "GET",
    headers: {
      Authorization: `Bearer ${token}`,
      Accept: "application/json",
      "OAI-Product-Sku": "codex_desktop",
    },
  });

  const text = await response.text();
  let body;
  try {
    body = text.length === 0 ? null : JSON.parse(text);
  } catch {
    body = null;
  }

  if (!response.ok) {
    const message = body?.error || body?.detail || body?.message || text.slice(0, 200) || "request failed";
    throw new Error(`Backend returned HTTP ${response.status}: ${message}`);
  }

  if (body == null || typeof body !== "object") {
    throw new Error("Backend returned an empty or non-JSON response");
  }

  return body;
}

async function main() {
  const args = parseArgs(process.argv.slice(2));
  if (args.help) {
    printHelp();
    return;
  }

  const token = await readAccessToken(args.authPath);
  const data = await fetchCredits(token);
  const credits = Array.isArray(data.credits) ? data.credits : [];
  const nowMs = Date.now();

  const output = {
    retrieved_at_utc: new Date(nowMs).toISOString(),
    endpoint: ENDPOINT,
    timezone: args.timezone,
    available_count: data.available_count ?? null,
    total_earned_count: data.total_earned_count ?? null,
    credit_count: credits.length,
    status_counts: countStatuses(credits),
    credits: credits.map((credit, index) => sanitizeCredit(credit, index, args.timezone, nowMs)),
  };

  console.log(JSON.stringify(output, null, 2));
}

main().catch((error) => {
  console.error(JSON.stringify({ error: error.message }, null, 2));
  process.exitCode = 1;
});

