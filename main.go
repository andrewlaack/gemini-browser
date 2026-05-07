package main

import (
	"io"
	"os"
	"time"
	"github.com/gdamore/tcell/v2"
	"github.com/makeworld-the-better-one/go-gemini"
	"github.com/rivo/tview"
)

func main(){

	client := &gemini.Client{ConnectTimeout: 5 * time.Second}
	resp, err := client.Fetch(os.Args[1])

	if err != nil {
		panic(err)
	}


	bodyBytes, err := io.ReadAll(resp.Body)
	resp.Body.Close()

	text := string(bodyBytes)

	// links := ParseLinks(text, os.Args[1])

	app := tview.NewApplication()

	textView := tview.NewTextView()
	textView.SetBackgroundColor(tcell.ColorDefault)
	textView.SetText(text)
	
	selectionMode := false

	textView.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {
			switch event.Rune() {
			case ' ':
				selectionMode = !selectionMode
			case '1', '2','3','4','5','6','7','8','9','0':
				if selectionMode {
					selectionMode = false
					go func() {
						client := &gemini.Client{ConnectTimeout: 5 * time.Second}
						resp, err := client.Fetch("gemini://blog.laack.co")

						if err != nil {
							panic(err)
						}

						bodyBytes, err := io.ReadAll(resp.Body)
						if err != nil {
							panic(err)
						}

						resp.Body.Close()

						textNew := string(bodyBytes)

						app.QueueUpdateDraw(func() {
							textView.SetText(textNew)
						})
					}()

					return nil
				}
			}

			return event
		})




	if err := app.SetRoot(textView, true).SetFocus(textView).Run(); err != nil {
		panic(err)
	}


}
