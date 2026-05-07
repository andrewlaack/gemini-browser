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
	text := string(bodyBytes)


	app := tview.NewApplication()

	textView := tview.NewTextView()
	textView.SetBackgroundColor(tcell.ColorDefault)

	textView.SetText(text)

	if err := app.SetRoot(textView, true).SetFocus(textView).Run(); err != nil {
		panic(err)
	}


}
