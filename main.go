package main

import (
	"io"
	"fmt"
	"net/url"
	"strconv"
	"strings"
	"github.com/gdamore/tcell/v2"
	"github.com/makeworld-the-better-one/go-gemini"
	"github.com/rivo/tview"
)

type Node struct {
	next *Node
	prior *Node
	url string
}

type Site struct {
	statusCode int
	siteContent string
	url string
}

type Link struct {
	address string
	plaintext string
}

var (
	// TODO: Possibly replace this with a cache; it will be annoying to persist the page position
	// but it's nice to have that when traversing links

	// TODO: There also seems to be some other form of state that messes with history traversal.

	history *Node
	linkSelectionText string
	links []Link
	site Site
	linkFollowMode bool
	mainArea *tview.Flex
	mainText *tview.TextView
	entryText *tview.TextView
	app *tview.Application
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

// TODO: Handle redirects
// seems like most clients prompt before redirects
// I'd want prompts prior to redirects cross-origin

func updateSite(newUrl string) (error){
	
	client := &gemini.Client{}

	resp, err := client.Fetch(newUrl)

	if err != nil {
		return err
	}

	defer resp.Body.Close()

	bodyBytes, err := io.ReadAll(resp.Body)

	if err != nil {
		return err
	}

	body := string(bodyBytes)

	newNode := &Node{url: newUrl}

	if history != nil {
		history.next = newNode
		newNode.prior = history
	} 

	history = newNode


	// TODO: Should be done once the site text is updated
	site.url = newUrl


	totalLinkCount := CountLinks(body)
	lines := strings.Split(body, "\n")

	escaped := false
	escape := "```"

	result := ""
	linkCount := 0


	links = []Link{}

	// this should never happen because we already
	// loaded this site and all that
	u, err := url.Parse(newUrl)

	if err != nil {
		panic(err)
	}

	for _, item := range lines {

		inserted := false

		if len(item) >= 3 && strings.Compare(escape, item[:3]) == 0 {
			escaped = !escaped
		}


		if len(item) > 3 && !escaped {
			if item[0] == '=' && item[1] == '>' {

				link := Link{}
				s := stripLeadingWhiteSpace(item[2:])

				parts := strings.FieldsFunc(s, func(r rune) bool {
					return r == ' ' || r == '\t'
				})

				address := "" 
				text := ""

				if len(parts) > 0 {
					address = parts[0]
				}
				if len(parts) > 1 {
					text = strings.Join(parts[1:], " ")
				}

				newLn, err := url.Parse(address)

				if err != nil {
					// invalid url on this site...
					// that's their fucking fault, leave it as is
					link.address = address
				} else {
					link.address = u.ResolveReference(newLn).String()
				}

				link.plaintext = strings.TrimSpace(sanitized(text))

				links = append(links, link)

				inserted = true

				spacer := " "
				if totalLinkCount >= 10 && linkCount < 10 {
					spacer = "  "
				}

				if link.plaintext != "" {
					result += fmt.Sprintf("[%d]%s=> %s", linkCount, spacer, link.plaintext)
				} else {
					// grr.... sanitizing the link fucks stuff up in terms of conssitency
					result += fmt.Sprintf("[%d]%s=> %s", linkCount, spacer, sanitized(link.address))
				}

				linkCount += 1
			}
		}

		if !inserted {
			result += sanitized(item)
		}

		result += "\n"

	}

	site.siteContent = result
	site.statusCode = resp.Status
	return nil
}

// This will only render ascii. Anything beyond this shouldn't be assumed to work in a conventional terminal
// Emojis don't work in st.

// Yes, this does break international support; consider patching if you want, but this broke st trying to
// re-render stuff, could be a limit of the combination of st and tview.

func sanitized(s string) string {
	var b strings.Builder

	for _, r := range s {
		if r <= 0x7F {
			b.WriteRune(r)
		}
	}

	return b.String()

}

func initApplication() *tview.Application {
	app := tview.NewApplication()
	return app
}

func repaint() {
    app.QueueUpdateDraw(func() {
        mainArea.SetTitle(site.url)
        mainText.SetText(site.siteContent)
		if linkFollowMode {
			entryText.SetText("Link to follow: "+linkSelectionText)
		} else {
			entryText.SetText("")
		}
    })
}

func main(){

	app = initApplication()
	mainText = tview.NewTextView()
	mainText.SetBackgroundColor(tcell.ColorDefault)
	entryText = tview.NewTextView()

	mainArea = tview.NewFlex().SetDirection(tview.FlexRow)
	mainArea.SetBorder(true)

	mainArea.AddItem(mainText, 0, 1, true) 
	mainArea.AddItem(entryText, 1, 0, false)
	entryText.SetText("")

	go func() {
		err := updateSite("gemini://tlgs.one/known-hosts")

		if err != nil {
			app.Stop()
			panic(err)
		}

		repaint()
	}()
	
	mainArea.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {
		go func() {

			if event.Key() == tcell.KeyEnter {
				if linkFollowMode {

					selection, err := strconv.Atoi(linkSelectionText)

					if err == nil {
						if selection < len(links) {
							// TODO: Handle this possible error 
							updateSite(links[selection].address)
						}
					}

					linkFollowMode = false
					linkSelectionText = ""
				}
			}

			if event.Key() == tcell.KeyEsc {
				if linkFollowMode {
					linkFollowMode = false
					linkSelectionText = ""

				} 
			}

			r := event.Rune()
			if r == '0' || r == '1' || r == '2' || r == '3' || r == '4' || r == '5' || r == '6' || r == '7' || r == '8' || r == '9'{
				if linkFollowMode {
					linkSelectionText += string(r)
				}
			}

			if r == 'b' {
				if history != nil && history.prior != nil && history.prior.url != "" {
					history = history.prior
					target := history.url

					// pop again to jump forwards correctly again when calling update site
					history = history.prior

					// TODO: handle this possible error
					updateSite(target)

				}
			}
			// TODO: Add forwards link traversal. This is slightly more difficult to not mess
			// up the history.

			if event.Rune() ==  ' '{
				linkFollowMode = true
			}

			// It's always safe to repaint; the entire state is stored globally, nothing is
			// stored in components

			repaint()
		}()

		return event
	})

	app.SetRoot(mainArea, true).SetFocus(mainArea)
	err := app.Run()

	if err != nil {
		panic(err)
	}
}

func CountLinks(body string) int {
	lines := strings.Split(body, "\n")
	count := 0

	escaped := false
	escape := "```"

	for _, item := range lines {
		if len(item) >= 3 && strings.Compare(escape, item[:3]) == 0 {
			escaped = !escaped
		}

		if len(item) > 3 && !escaped {
			if item[0] == '=' && item[1] == '>' {
				count += 1
			}
		}

	}

	return count
}
