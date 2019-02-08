/** Handle sigpipe internally */
Sys.(set_signal(sigpipe, Signal_ignore));

/** Setup loggers */
Fmt_tty.setup_std_outputs();
Logs.set_level(Some(Logs.Debug));
Logs.set_reporter(Logs_fmt.reporter());

let port = 2112;
let on_start = () =>
  Logs.app(m => m("Running on localhost:%s", port |> string_of_int));
/** Start server */
Server.make(~port, ~on_start);
