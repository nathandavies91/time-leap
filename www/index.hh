<?hh // strict

/**
 * Time Leap
 * @author Nathan Davies, http://nathandavies.ninja
 * @copyright Copyright (c) 2015, Nathan Davies
 */

class TimeLeap
{
    /**
     * @var DateTime
     */
    protected DateTime $leapDay, $leapSecond, $today;
    
    /**
     * @var string
     */
    protected string $date, $format = "l, jS F Y", $type;
    
    /**
     * Constructor
     */
    public function __construct(): void
    {
        $this->today = new Datetime;
        
        // Next event
        $this->date = $this->isLeapDayNext() ? $this->getNextLeapDay()->format($this->format) : $this->getNextLeapSecond()->format($this->format);
        $this->type = $this->isLeapDayNext() ? "Day" : "Second";
        
        // Display
        echo $this->getHTML();
    }
    
    /**
     * Retrieve contents of the requested file
     * @param string $file The file
     * @return string
     */
    protected function getFile(string $file): ?string
    {
        return file_exists("../assets/" . $file) ? file_get_contents("../assets/" . $file) : null;
    }
    
    /**
     * The complete HTML
     * @return string
     */
    protected function getHTML(): string
    {
        if (!is_null(($file = $this->getFile("layout.html"))))
            return preg_replace(['/\{date\}/', '/\{type\}/'], [$this->date, $this->type], $file);
        else
            return $this->date . " (Leap " . $this->type . ")\n";
    }
    
    /**
     * The date of the next leap day
     * @return DateTime
     */
    protected function getNextLeapDay(): DateTime
    {
        if (is_null($this->leapDay))
        {
            $month = $this->today->format("n");
            $year  = $this->today->format("Y");
            return new DateTime(((!($year % 4 > 0) && $month <= 2) ? $year : $year + (4 - ($year % 4))) . "-02-29");
        }
        
        return $this->leapDay;
    }
    
    /**
     * The date of the next leap second
     * @return DateTime
     */
    protected function getNextLeapSecond(): ?DateTime
    {
        if (is_null($this->leapSecond))
        {
            try {
                $leapSeconds = json_decode($this->getFile("seconds.json"), true);
                $count = count($leapSeconds);
                
                // Grab the next active event
                for ($i = 0; $i < $count; $i++)
                {
                    $ls = new DateTime($leapSeconds[$i]['time']);
                    
                    // Outdated?
                    if ($ls < $this->today)
                        unset($leapSeconds[$i]);
                    
                    // Next event?
                    else
                    {
                        $this->leapSecond = $ls;
                        break;
                    }
                }
                
                // Tidy up file
                if (count($leapSeconds) < $count)
                    file_put_contents("../assets/seconds.json", json_encode($leapSeconds));
            }
            catch (Exception $e)
            {
                // Damn
            }
        }
        
        return $this->leapSecond;
    }
    
    /**
     * Is the leap day the next type of event?
     * @return bool
     */
    protected function isLeapDayNext(): bool
    {
        return (is_null($this->getNextLeapSecond()) || $this->getNextLeapDay() <= $this->getNextLeapSecond());
    }
} new TimeLeap;