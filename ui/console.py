import threading
from rich.text import Text
from textual.app import App, ComposeResult
from textual.widgets import Header, Footer, Tree
from textual.containers import Container
from textual.widgets import Input, ListView, ListItem, Footer, Header, Static
from bt.service import Service
import asyncio
import io, sys

class RedirectOutput:
    def __init__(self, callback):
        self.callback = callback
        self.output_buffer = io.StringIO()
        self.original_stdout = sys.stdout

    def start(self):        
        sys.stdout = self
        print("test")

    def stop(self):        
        sys.stdout = self.original_stdout        
        captured_output = self.output_buffer.getvalue()
        self.callback(captured_output)

    def write(self, message):        
        self.original_stdout.write(message)
        self.output_buffer.write(message)
        self.callback(message)

    def flush(self):
        pass

class TreeApp(App):
    CSS_PATH = "console.tcss"

    BINDINGS = [
        ("a", "auto_scroll", "Auto scroll"),
        ("c", "clear", "Clear"),
        ("t", "toggle_root", "Toggle root"),
    ]
    
    auto_scroll = True



    def __init__(self, service:Service,  packets: dict):
        super().__init__()
        self.redirector = RedirectOutput(self.handle_print)    
        self.packets = packets
        self.service = service
        self.service.subscribe(self.handle_message)


    async def on_ready(self):
        self.redirector.start()
        print("Welcome to the Textual app!")
        print("This is redirected output.")

        def run_loop(loop):
            asyncio.set_event_loop(loop)
            loop.run_forever()


        loop = asyncio.new_event_loop()
        thread = threading.Thread(target=run_loop, args=(loop,))
        thread.start()
        asyncio.run_coroutine_threadsafe(self.service.start(), loop)


    def compose(self) -> ComposeResult:

        yield Header()
        self.title = "Header Application"
        self.sub_title = "With title and sub-title"

        tree = Tree("AAP packets", id="tree-view")

        def create_note(root, label, data):
            node = root.add("")
            node.allow_expand = False
            node.data = data
            node.label = label

        for category, items in self.packets.items():            
            if isinstance(items, list):
                json_node = tree.root.add(category)
                for label, data in items:
                    create_note(json_node, label, data)             
            else:
                create_note(tree.root, category, items)                
        tree.root.expand()
        tree.show_root = not tree.show_root
        
        with Container():
            yield tree            
            self.list_view = ListView(id="code-view")
            with Container():
                yield self.list_view
                yield Input(placeholder="hex packet to send", id="dictionary-search")
        
        yield Footer()
    
    async def on_tree_node_selected(self, event:Tree.NodeSelected) -> None:                        
        node = event.node        
        if node.allow_expand == False:
            #self.pprint(node.label)
            print(node.label)
    
    def pprint(self, text, color = "") -> None:                        
        self.list_view.append(ListItem(Static(Text(str(text), style=color))))
        if self.auto_scroll:
            self.list_view.scroll_to(y=len(self.list_view.children) - 1)


    def action_auto_scroll(self) -> None:
        self.auto_scroll = not self.auto_scroll
        self.pprint(f"Auto scroll {self.auto_scroll}", "red")

    def on_input_submitted(self, event: Input.Submitted) -> None:    
        input_text =  event.input.value
        self.pprint(input_text, "green")
        event.input.value = ""

    def action_clear(self) -> None:
        self.list_view.clear()

    def action_toggle_root(self) -> None:
        tree = self.query_one(Tree)
        tree.show_root = not tree.show_root

    def handle_print(self, message):        
        self.pprint(message.strip())

    def handle_message(self, message):
        self.pprint(message.hex(), color="bright_cyan")