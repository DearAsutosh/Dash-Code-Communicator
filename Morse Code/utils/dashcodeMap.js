// utils/dashcodeMap.js
// Modified: robust decoder (supports '|' delimited or greedy parsing), encoder, and both mappings.

const dashToChar = {
  "***": "A",
  "-*.": "B",
  "-**": "C",
  "*-*": "D",
  "...": "E",
  "*.-": "F",
  "..-": "G",
  "*..": "H",
  ".**": "I",
  "--.": "J",
  ".--": "K",
  "-..": "L",
  ".-*": "M",
  "**.": "N",
  ".*.": "O",
  "-.*": "P",
  "*--": "Q",
  "**-": "R",
  "*.*": "S",
  "..*": "T",
  ".*-": "U",
  "-.-": "V",
  "*-.": "W",
  "-*-": "X",
  ".-.": "Y",
  "--*": "Z",
  "---": "0",
  "#..": "1",
  "#.*": "2",
  "#*.": "3",
  "#**": "4",
  "#--": "5",
  "#-.": "6",
  "#.-": "7",
  "#*-": "8",
  "#-*": "9",
};

// build reverse map (char -> dash)
const charToDash = Object.keys(dashToChar).reduce((acc, k) => {
  const ch = dashToChar[k];
  acc[ch] = k;
  return acc;
}, {});

/**
 * Normalizes input: trims, collapses multiple spaces to single space.
 */
function _normalize(sentence) {
  if (!sentence || typeof sentence !== "string") return "";
  return sentence.trim().replace(/\s+/g, " ");
}

/**
 * Decode a dash-code sentence into human readable text.
 *
 * Supported input formats:
 * 1) Words separated by spaces, letters separated by '|' (default expected):
 *    "***|-*.|... ..-|.-."  -> "ABE UR"
 * 2) Words separated by spaces, letters NOT delimited:
 *    "***-*... ..|.-."      -> Greedy longest-match parser (tries 3->2->1 length)
 *
 * Options:
 *  - letterSep: if provided (e.g. '|'), will use it as letter separator; if null will auto-detect '|' or greedy-parse.
 */
function fromDashCode(sentence, options = {}) {
  const letterSep = options.letterSep === undefined ? null : options.letterSep;
  sentence = _normalize(sentence);
  if (sentence === "") return "";

  const words = sentence.split(" ");
  const decoded = words.map((word) => {
    // if explicit separator provided or present in word, use split mode
    const sepToUse = letterSep || (word.includes("|") ? "|" : null);
    if (sepToUse) {
      return word
        .split(sepToUse)
        .map((code) => dashToChar[code] || "?")
        .join("");
    }

    // Greedy parse: try longest match first (max code length = 3)
    let out = "";
    let i = 0;
    while (i < word.length) {
      let matched = false;
      // Try lengths 3,2,1
      for (let len = 3; len >= 1; len--) {
        const part = word.substr(i, len);
        if (dashToChar.hasOwnProperty(part)) {
          out += dashToChar[part];
          i += len;
          matched = true;
          break;
        }
      }
      if (!matched) {
        // unknown fragment: insert '?' and advance 1 to avoid infinite loop
        out += "?";
        i += 1;
      }
    }
    return out;
  });

  return decoded.join(" ");
}

/**
 * Encode a human-readable text into DashCode.
 *
 * By default letters are separated by '|' and words by space.
 * Options:
 *  - letterSep: string used to separate letters in output (default: '|').
 */
function toDashCode(text, options = {}) {
  const letterSep = options.letterSep === undefined ? "|" : options.letterSep;
  if (!text || typeof text !== "string") return "";

  const words = text.toUpperCase().trim().split(/\s+/);
  const encodedWords = words.map((word) => {
    return word
      .split("")
      .map((ch) => {
        // allow digits and A-Z only; unknown -> '?'
        return charToDash[ch] || "?";
      })
      .join(letterSep);
  });

  return encodedWords.join(" ");
}

module.exports = {
  dashToChar,
  charToDash,
  fromDashCode,
  toDashCode,
};

// HELP ME : *.. ... -.. -.*  .-* ...
// SOS : *.* .*. *.*
// EMERGENCY : ... .-* ... **- ..- ... **. -** .-.
// ATTACK : *** -.- -.- .-* -.-
// SAFE : *** *.. *.- ** ...
// RETREAT : **- **. **- ... *.. ***
