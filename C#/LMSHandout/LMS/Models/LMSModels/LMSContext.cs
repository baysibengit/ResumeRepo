using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore;
using Pomelo.EntityFrameworkCore.MySql.Scaffolding.Internal;

namespace LMS.Models.LMSModels;

public partial class LMSContext : DbContext
{
    public LMSContext()
    {
    }

    public LMSContext(DbContextOptions<LMSContext> options)
        : base(options)
    {
    }

    public virtual DbSet<Administrator> Administrators { get; set; }

    public virtual DbSet<Assignment> Assignments { get; set; }

    public virtual DbSet<AssignmentCategory> AssignmentCategories { get; set; }

    public virtual DbSet<Class> Classes { get; set; }

    public virtual DbSet<Course> Courses { get; set; }

    public virtual DbSet<Department> Departments { get; set; }

    public virtual DbSet<Enrollment> Enrollments { get; set; }

    public virtual DbSet<Professor> Professors { get; set; }

    public virtual DbSet<Student> Students { get; set; }

    public virtual DbSet<Submission> Submissions { get; set; }

    protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        => optionsBuilder.UseMySql("name=LMS:LMSConnectionString", Microsoft.EntityFrameworkCore.ServerVersion.Parse("10.11.8-mariadb"));

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder
            .UseCollation("latin1_swedish_ci")
            .HasCharSet("latin1");

        modelBuilder.Entity<Administrator>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.Property(e => e.Id)
                .HasMaxLength(8)
                .HasColumnName("ID");
            entity.Property(e => e.Dob).HasColumnName("DOB");
            entity.Property(e => e.First).HasMaxLength(100);
            entity.Property(e => e.Last).HasMaxLength(100);
        });

        modelBuilder.Entity<Assignment>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.HasIndex(e => e.Category, "Category");

            entity.HasIndex(e => new { e.Name, e.Category }, "Name").IsUnique();

            entity.Property(e => e.Id)
                .HasColumnType("int(10) unsigned")
                .HasColumnName("ID");
            entity.Property(e => e.Category).HasColumnType("int(10) unsigned");
            entity.Property(e => e.Due).HasColumnType("datetime");
            entity.Property(e => e.Instructions).HasColumnType("text");
            entity.Property(e => e.Name).HasMaxLength(100);
            entity.Property(e => e.Points).HasColumnType("int(10) unsigned");

            entity.HasOne(d => d.CategoryNavigation).WithMany(p => p.Assignments)
                .HasForeignKey(d => d.Category)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Assignments_ibfk_1");
        });

        modelBuilder.Entity<AssignmentCategory>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.HasIndex(e => e.Class, "Class");

            entity.HasIndex(e => new { e.Name, e.Class }, "Name").IsUnique();

            entity.Property(e => e.Id)
                .HasColumnType("int(10) unsigned")
                .HasColumnName("ID");
            entity.Property(e => e.Class).HasColumnType("int(10) unsigned");
            entity.Property(e => e.Name).HasMaxLength(100);
            entity.Property(e => e.Weight).HasColumnType("tinyint(3) unsigned");

            entity.HasOne(d => d.ClassNavigation).WithMany(p => p.AssignmentCategories)
                .HasForeignKey(d => d.Class)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("AssignmentCategories_ibfk_1");
        });

        modelBuilder.Entity<Class>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.HasIndex(e => e.Course, "Course");

            entity.HasIndex(e => e.Professor, "Professor");

            entity.HasIndex(e => new { e.Year, e.Semester, e.Course }, "Semester").IsUnique();

            entity.Property(e => e.Id)
                .HasColumnType("int(10) unsigned")
                .HasColumnName("ID");
            entity.Property(e => e.Course).HasColumnType("int(10) unsigned");
            entity.Property(e => e.EndTime).HasColumnType("time");
            entity.Property(e => e.Location).HasMaxLength(100);
            entity.Property(e => e.Professor)
                .HasMaxLength(8)
                .IsFixedLength();
            entity.Property(e => e.Semester).HasMaxLength(6);
            entity.Property(e => e.StartTime).HasColumnType("time");
            entity.Property(e => e.Year).HasColumnType("int(10) unsigned");

            entity.HasOne(d => d.CourseNavigation).WithMany(p => p.Classes)
                .HasForeignKey(d => d.Course)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Classes_ibfk_1");

            entity.HasOne(d => d.ProfessorNavigation).WithMany(p => p.Classes)
                .HasForeignKey(d => d.Professor)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Classes_ibfk_2");
        });

        modelBuilder.Entity<Course>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.HasIndex(e => e.Dept, "Dept");

            entity.HasIndex(e => new { e.Number, e.Dept }, "Number").IsUnique();

            entity.Property(e => e.Id)
                .HasColumnType("int(10) unsigned")
                .HasColumnName("ID");
            entity.Property(e => e.Dept).HasMaxLength(4);
            entity.Property(e => e.Name).HasMaxLength(100);
            entity.Property(e => e.Number).HasColumnType("smallint(6)");

            entity.HasOne(d => d.DeptNavigation).WithMany(p => p.Courses)
                .HasForeignKey(d => d.Dept)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Courses_ibfk_1");
        });

        modelBuilder.Entity<Department>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.Property(e => e.Id)
                .HasMaxLength(4)
                .HasColumnName("ID");
            entity.Property(e => e.Name).HasMaxLength(100);
        });

        modelBuilder.Entity<Enrollment>(entity =>
        {
            entity.HasKey(e => new { e.Student, e.Class })
                .HasName("PRIMARY")
                .HasAnnotation("MySql:IndexPrefixLength", new[] { 0, 0 });

            entity.HasIndex(e => e.Class, "Class");

            entity.Property(e => e.Student).HasMaxLength(8);
            entity.Property(e => e.Class).HasColumnType("int(10) unsigned");
            entity.Property(e => e.Grade).HasMaxLength(2);

            entity.HasOne(d => d.ClassNavigation).WithMany(p => p.Enrollments)
                .HasForeignKey(d => d.Class)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Enrollments_ibfk_2");

            entity.HasOne(d => d.StudentNavigation).WithMany(p => p.Enrollments)
                .HasForeignKey(d => d.Student)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Enrollments_ibfk_1");
        });

        modelBuilder.Entity<Professor>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.HasIndex(e => e.Dept, "Dept");

            entity.Property(e => e.Id)
                .HasMaxLength(8)
                .HasColumnName("ID");
            entity.Property(e => e.Dept).HasMaxLength(4);
            entity.Property(e => e.Dob).HasColumnName("DOB");
            entity.Property(e => e.First).HasMaxLength(100);
            entity.Property(e => e.Last).HasMaxLength(100);

            entity.HasOne(d => d.DeptNavigation).WithMany(p => p.Professors)
                .HasForeignKey(d => d.Dept)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Professors_ibfk_1");
        });

        modelBuilder.Entity<Student>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("PRIMARY");

            entity.HasIndex(e => e.Major, "Major");

            entity.Property(e => e.Id)
                .HasMaxLength(8)
                .HasColumnName("ID");
            entity.Property(e => e.Dob).HasColumnName("DOB");
            entity.Property(e => e.First).HasMaxLength(100);
            entity.Property(e => e.Last).HasMaxLength(100);
            entity.Property(e => e.Major).HasMaxLength(4);

            entity.HasOne(d => d.MajorNavigation).WithMany(p => p.Students)
                .HasForeignKey(d => d.Major)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Students_ibfk_1");
        });

        modelBuilder.Entity<Submission>(entity =>
        {
            entity.HasKey(e => new { e.Student, e.Assignment })
                .HasName("PRIMARY")
                .HasAnnotation("MySql:IndexPrefixLength", new[] { 0, 0 });

            entity.HasIndex(e => e.Assignment, "Assignment");

            entity.Property(e => e.Student).HasMaxLength(8);
            entity.Property(e => e.Assignment).HasColumnType("int(10) unsigned");
            entity.Property(e => e.Contents).HasColumnType("text");
            entity.Property(e => e.Score).HasColumnType("int(10) unsigned");
            entity.Property(e => e.Time).HasColumnType("datetime");

            entity.HasOne(d => d.AssignmentNavigation).WithMany(p => p.Submissions)
                .HasForeignKey(d => d.Assignment)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Submissions_ibfk_2");

            entity.HasOne(d => d.StudentNavigation).WithMany(p => p.Submissions)
                .HasForeignKey(d => d.Student)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("Submissions_ibfk_1");
        });

        OnModelCreatingPartial(modelBuilder);
    }

    partial void OnModelCreatingPartial(ModelBuilder modelBuilder);
}
