use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str, count: usize)
{
	assert!(line.len() >= count);
	let mut window_len: usize = 0;

	for (i, a) in line.char_indices() {
		match line[(i + 1)..].chars()
				.take(count - window_len - 1)
				.position(|b|a == b) {
			Some(_p) => {
				window_len = 0;
				continue;
			}
			None => {
				window_len += 1;
			}
		}

		if window_len == count {
			println!("{}", i + 1);
			return;
		}
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		parse_line(&line.expect("Invalid line"), 14);
	}
}
