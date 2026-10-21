# Completion notification test recording

User-supplied recording: https://www.mediafire.com/file/1i8wss1jcd5sqx5/Screen_Recording_20261018_163513_LeviLauncher.mp4

The recording was used only as visual test data. Sampled frames show Bedrock recipe-unlock toasts such as “New recipe unlocked! View in crafting table” and “New recipe unlocked! View in furnace”; no Advancements popup or custom chat message is visible. The advancement panel still shows entries as locked. This confirms that the current implementation must not treat recipe toasts as advancement completions, and that its Achievement-toast text scan has not produced a reliable completion event in this test.
