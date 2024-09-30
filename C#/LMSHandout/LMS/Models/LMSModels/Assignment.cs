using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Assignment
{
    public uint Id { get; set; }

    public string Name { get; set; } = null!;

    public DateTime Due { get; set; }

    public uint Points { get; set; }

    public string? Instructions { get; set; }

    public uint Category { get; set; }

    public virtual AssignmentCategory CategoryNavigation { get; set; } = null!;

    public virtual ICollection<Submission> Submissions { get; set; } = new List<Submission>();
}
