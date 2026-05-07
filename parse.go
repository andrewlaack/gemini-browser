package main

import (
	"fmt"
	"net/url"
	"strings"
)

func stripLeadingWhiteSpace(text string) string {

	for len(text) > 0 {
		if text[0] == ' ' || text[0] == '\t' {
			if len(text) > 1 {
				text = text[1:]
			} else {
				text = ""
				return text
			}
		} else {
			return text
		}
	}

	return text
}

func ParseLinks(body string, currentUrl string) []string {

	base, err := url.Parse(currentUrl)

	if err != nil {
		panic(err)
	}

	lines := strings.Split(body, "\n")

	links := []string{}

	escaped := false
	escape := "```"

	for _, item := range lines {

		// must start with escape characters, the rest doesn't matter to us
		// > Any line whose first three characters are "```" (...) are preformatted toggle lines
		if len(item) >= 3 && strings.Compare(escape, item[:3]) == 0 {
			escaped = !escaped
		}

		if len(item) > 3 && !escaped {
			if item[0] == '=' && item[1] == '>' {

				// sometimes links end with a \r, but that isn't valid so we won't allow it
				links = append(links, stripLeadingWhiteSpace(item[2:]))
			}
		}

	}

	geminiLinks := []string{}

	for _, item := range links {

		// this is for finding the text associated with the link

		indexOfSpace := strings.Index(item, " ")
		indexOfTab := strings.Index(item, "\t")

		// default if there aren't any
		indexOfSpaceOrTab := len(item)

		if indexOfSpace != -1 {
			indexOfSpaceOrTab = indexOfSpace
		}

		if indexOfTab != -1 {
			if indexOfTab < indexOfSpace || indexOfSpace == -1 {
				indexOfSpaceOrTab = indexOfTab
			}
		}

		if len(item) >= 10 && strings.Compare(item[:9], "gemini://") == 0 {
			geminiLinks = append(geminiLinks, item[0:indexOfSpaceOrTab])
		}

		// there are urls that aren't relative that don't have // like like mailto: and monero:
		if strings.Contains(item, ":") == false {
			// relative link

			u, err := url.Parse(item[0:indexOfSpaceOrTab])

			if err != nil {
				fmt.Printf("Unable to parse link: %s, %s\n", item, err)
				continue
			}

			geminiLinks = append(geminiLinks, base.ResolveReference(u).String())
		}
	}

	return geminiLinks
}
