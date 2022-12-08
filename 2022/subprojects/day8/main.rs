use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str, trees: &mut Vec<Vec<u32>>)
{
	let mut row: Vec<u32> = Vec::new();

	for tree in line.chars() {
		row.push(tree.to_digit(10).expect("Invalid number"));
	}

	trees.push(row);
}

fn is_visible(trees: &Vec<Vec<u32>>, col: usize, row: usize) -> bool
{
	let height = trees[row][col];
	let max_row = trees.len();
	let max_col = trees[row].len();
	let mut visible: bool = true;

	for c in 0..col {
		if trees[row][c] >= height {
			visible = false;
			break;
		}
	}
	
	if visible == true {
		return true;
	}

	visible = true;

	for c in (col + 1)..max_col {
		if trees[row][c] >= height {
			visible = false;
			break;
		}
	}
	
	if visible == true {
		return true;
	}

	visible = true;

	for r in 0..row {
		if trees[r][col] >= height {
			visible = false;
			break;
		}
	}

	if visible == true {
		return true;
	}

	visible = true;

	for r in (row + 1)..max_row {
		if trees[r][col] >= height {
			visible = false;
			break;
		}
	}
	
	if visible == true {
		return true;
	}

	false
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut trees: Vec<Vec<u32>> = Vec::new();
	let mut visibility: u32 = 0;

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		parse_line(&line.expect("Invalid line"), &mut trees);
	}

	for (r, row) in trees.iter().enumerate() {
		for (c, _col) in row.iter().enumerate() {
			let visible = is_visible(&trees, c, r);
			if visible {
				visibility += 1;
			}
		}
	}

	println!("{:?}", visibility);
}
