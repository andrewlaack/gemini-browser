package main

import (
	"io"
	"fmt"
	"os"
	"strconv"
	"time"
	"github.com/gdamore/tcell/v2"
	"github.com/makeworld-the-better-one/go-gemini"
	"github.com/rivo/tview"
)

func main(){

	client := &gemini.Client{ConnectTimeout: 5 * time.Second}
	currentUrl := os.Args[1]
	resp, err := client.Fetch(currentUrl)

	if err != nil {
		panic(err)
	}


	bodyBytes, err := io.ReadAll(resp.Body)
	resp.Body.Close()

	text := string(bodyBytes)

	app := tview.NewApplication()

	textView := tview.NewTextView()
	textView.SetBackgroundColor(tcell.ColorDefault)
	textView.SetText(text)
	lowerTextView := tview.NewTextView()
	screen := tview.NewFlex().SetDirection(tview.FlexRow).AddItem(textView, 0,1,true).AddItem(lowerTextView,1,0,false)
	
	selectionMode := false
	currentUrlSelection := ""

	textView.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {

		switch event.Rune() {
		case ' ':
			selectionMode = !selectionMode
			go func() {
				app.QueueUpdateDraw(func() {
					if selectionMode {
						lowerTextView.SetText("Select Link To Follow: ")
					} else{
						lowerTextView.SetText("")
					}
				})

			}()

					return nil
			case '1', '2','3','4','5','6','7','8','9','0':
			if selectionMode {
				go func() {

					currentUrlSelection += string(event.Rune())

					app.QueueUpdateDraw(func() {
						currentText := lowerTextView.GetText(false)
						currentText = currentText + string(event.Rune())
						lowerTextView.SetText(currentText)
					})

				}()
			}

		}

		switch event.Key() {
			case tcell.KeyEnter:
				if selectionMode {
					selectionMode = false
					go func() {
						client := &gemini.Client{ConnectTimeout: 5 * time.Second}

						links := ParseLinks(text, currentUrl)

						currentSelectionNum, err := strconv.Atoi(currentUrlSelection)
						currentUrlSelection = ""

						if err != nil {
							panic(err)
						}

						if len(links) > currentSelectionNum {
							currentUrl = links[currentSelectionNum]
							resp, err := client.Fetch(currentUrl)

							if err != nil {
								panic(err)
							}

							bodyBytes, err := io.ReadAll(resp.Body)
							if err != nil {
								panic(err)
							}

							resp.Body.Close()

							text = string(bodyBytes)

							app.QueueUpdateDraw(func() {
								textView.SetText(text)
								lowerTextView.SetText(currentUrl)
							})
						} else {
							app.QueueUpdateDraw(func() {
								lowerTextView.SetText(fmt.Sprintf("Links: %v", links))
							})

						}

					}()

					return nil
				}
			}

		return event
	})



	if err := app.SetRoot(screen, true).SetFocus(textView).Run(); err != nil {
		panic(err)
	}


}
