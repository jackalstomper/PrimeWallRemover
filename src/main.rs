mod shapes;

use crate::shapes::Pak;
use byteorder::{BigEndian, ReadBytesExt, WriteBytesExt};
use nod::{
    common::PartitionKind,
    read::{DiscOptions, DiscReader, PartitionOptions},
};
use std::fs::OpenOptions;
use std::io::{self, Cursor, Read, Seek, Write};

const MREA_MAGIC: u32 = 0xDEADBEEF;
const COLLISION_MAGIC: u32 = 0xDEAFBABE;
const BROKEN_RELAY_ADDR: u64 = 0x4CE1ABA0;

// Material flag shift values
const FLOOR: u32 = 31;
const WALL: u32 = 30;
const CEILING: u32 = 29;
const SOLID: u32 = 19;
const SCAN_THRU: u32 = 27;

fn read_input() -> io::Result<String> {
    let stdin = io::stdin();
    let mut input_buff = String::new();
    stdin.read_line(&mut input_buff)?;
    Ok(input_buff.trim().into())
}

fn main() -> io::Result<()> {
    println!("Enter ISO location");
    print!("> ");
    io::stdout().flush()?;
    let iso_path = read_input()?;
    if !std::fs::exists(&iso_path)? {
        println!("Invalid ISO path. No file found at {}", iso_path);
        return Ok(());
    }
    println!(
        "This tool will modify {} in-place to remove walls. Make sure you have a vanilla backup.",
        iso_path
    );
    println!("Do you want to continue? y/N");
    print!("> ");
    io::stdout().flush()?;
    let confirmed = read_input()?;
    if confirmed != "y" && confirmed != "Y" {
        println!("Exiting.");
        return Ok(());
    }
    let disc = DiscReader::new(&iso_path, &DiscOptions::default()).expect("Failed to open disc");
    // Open the image to write modifications to
    let mut outfile = OpenOptions::new()
        .write(true)
        .open(iso_path)
        .expect("Failed to open disc");
    let mut partition = disc
        .open_partition_kind(PartitionKind::Data, &PartitionOptions::default())
        .expect("Failed to open data partition");
    let meta = partition.meta().expect("Failed to read partition metadata");
    if meta.disc_header().game_id_str() != "GM8E01" {
        println!("Invalid game image. Must be GM8E01 (Metroid Prime)");
        return Ok(());
    }
    let fst = meta.fst().expect("File system table is invalid");
    let mut mod_count = 0;
    println!("Searching for walls to remove...");
    for (_, node, filename) in fst
        .iter()
        .filter(|e| e.2.starts_with("Metroid") || e.2.starts_with("metroid"))
    {
        let mut data = Vec::new();
        partition
            .open_file(node)
            .expect(&format!("Failed to open file stream for {}", &filename))
            .read_to_end(&mut data)
            .expect(&format!("Failed to read {}", &filename));
        let count = process_pak(Cursor::new(&mut data))?;
        if count > 0 {
            // We don't modify size of file, just flipping bits.
            // So we can write back to the image without issue.
            println!("Modified {}", &filename);
            outfile.seek(io::SeekFrom::Start(node.offset(false)))?;
            outfile.write_all(&data)?;
            mod_count += count;
        }
    }
    // There is a particular relay in Aeter Lab Entryway that triggers a load for East Tower
    // Problem is Retro disabled it. Either accidentally or on purpose we'll never know.
    // This means players need to force load the room by shooting the door with wave beam.
    // This is no fun for routing, so lets enable that relay again.
    outfile.seek(io::SeekFrom::Start(BROKEN_RELAY_ADDR))?;
    outfile.write_u8(1)?;
    if mod_count == 0 {
        println!("No walls found to remove. Was this ISO already modded?");
    } else {
        println!("Finished modifying ISO. Modified {} surfaces.", mod_count);
    }
    Ok(())
}

// https://wiki.axiodl.com/w/PAK_(Metroid_Prime)
fn process_pak<T: Read + Write + Seek>(mut buf: T) -> io::Result<u32> {
    let pak = Pak::read(&mut buf)?;
    let mut mod_count = 0;
    for e in pak.resources.iter().filter(|e| e.asset_type == "MREA") {
        buf.seek(io::SeekFrom::Start(e.offset as u64))?;
        let mut mrea_buf = vec![0; e.size as usize];
        buf.read_exact(&mut mrea_buf)?;
        mod_count += process_mrea(Cursor::new(&mut mrea_buf))?;
        buf.seek(io::SeekFrom::Start(e.offset as u64))?;
        buf.write_all(&mrea_buf)?;
    }
    Ok(mod_count)
}

// https://wiki.axiodl.com/w/MREA_(Metroid_Prime)
fn process_mrea<T: Read + Write + Seek>(mut buf: T) -> io::Result<u32> {
    if buf.read_u32::<BigEndian>()? != MREA_MAGIC {
        return Err(io::Error::new(
            io::ErrorKind::InvalidData,
            "Invalid MREA magic",
        ));
    }
    buf.seek(io::SeekFrom::Start(0x3C))?;
    let data_count = buf.read_u32::<BigEndian>()? as usize;
    buf.seek(io::SeekFrom::Start(0x48))?;
    let collision_index = buf.read_u32::<BigEndian>()? as usize;
    buf.seek(io::SeekFrom::Start(0x60))?;
    let mut data_sizes = vec![0; data_count];
    for i in 0..data_count {
        data_sizes[i] = buf.read_u32::<BigEndian>()?;
    }
    // Header ends at the next 32 byte boundry
    let pos = buf.stream_position()?;
    if pos % 32 != 0 {
        buf.seek(io::SeekFrom::Start(pos + 32 - pos % 32))?;
    }
    for i in 0..collision_index {
        buf.seek_relative(data_sizes[i] as i64)?;
    }
    process_collsion(buf)
}

// https://wiki.axiodl.com/w/Area_Collision_(File_Format)
fn process_collsion<T: Read + Write + Seek>(mut buf: T) -> io::Result<u32> {
    buf.seek_relative(8)?;
    if buf.read_u32::<BigEndian>()? != COLLISION_MAGIC {
        return Err(io::Error::new(
            io::ErrorKind::InvalidData,
            "Invalid collision section magic",
        ));
    }
    // Skip to collision index data
    buf.seek_relative(4 + 0x18 + 4)?;
    let octree_size = buf.read_u32::<BigEndian>()?;
    buf.seek_relative(octree_size as i64)?;
    let mat_count = buf.read_u32::<BigEndian>()?;
    let mut mod_count = 0;
    for _ in 0..mat_count {
        let mut flags = buf.read_u32::<BigEndian>()?;
        let is_floor = (flags >> FLOOR) & 1;
        let is_solid = (flags >> SOLID) & 1;
        let is_wall = (flags >> WALL) & 1;
        let is_ceiling = (flags >> CEILING) & 1;
        if is_floor == 0 && is_solid == 1 && (is_wall == 1 || is_ceiling == 1) {
            flags |= 1 << SCAN_THRU;
            flags ^= 1 << SOLID;
            buf.seek_relative(-4)?;
            buf.write_u32::<BigEndian>(flags)?;
            mod_count += 1;
        }
    }
    Ok(mod_count)
}
